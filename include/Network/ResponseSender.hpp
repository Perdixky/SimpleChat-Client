#pragma once
#include "Logic/LogicType.hpp"
#include "Network/Concepts.hpp"
#include "Network/MessageType.hpp"
#include "Network/ResponseRoute.hpp"
#include "Utils/GenericFormatter.hpp"
#include "Utils/Logger.hpp"
#include "rfl/Generic.hpp"
#include "rfl/from_generic.hpp"
#include "rfl/msgpack/write.hpp"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <expected>
#include <print>
#include <rfl.hpp>
#include <stdexec/execution.hpp>

namespace Network {

template <stdexec::receiver Receiver, RequestType T, ResponseRouterType R>
struct ResponseOperation {
  using ResponseType = T::Response;

  Receiver receiver_;

  R &router_;

  boost::uuids::uuid uuid_;

  auto start() noexcept -> void {
    auto callback = [this, r = std::move(receiver_)](
                        const std::expected<rfl::Generic::Object, ResponseError>
                            &result) mutable {
      if (!result) {
        log(error, "Request failed for UUID: {} with error: {}",
                   boost::uuids::to_string(uuid_),
                   static_cast<int>(result.error()));
        stdexec::set_error(
            std::move(r),
            std::make_exception_ptr(std::runtime_error("Request error")));
        return;
      }

      auto value = result.value(); // 这里一定有值

      if constexpr (requires { ResponseType::error; }) {
        if (auto err = value.get("error"); err) {
          log(warning, "Received error in response for UUID: {}, error: {}",
                       boost::uuids::to_string(uuid_),
                       err->to_string().value());
          stdexec::set_error(std::move(r),
                             std::make_exception_ptr(
                                 std::runtime_error(err->to_string().value())));
          return;
        }
      }

      auto response = rfl::from_generic<ResponseType>(value);
      if (!response) [[unlikely]] {
        log(error,
            "Failed to convert response from generic object for UUID: {}",
            boost::uuids::to_string(uuid_));
        log(error, "Expected error: {}", response.error().what());
        log(debug, "Generic object content:");
        log(debug, "{}", value);
        log(debug, "Expected response type: ");
        for (const auto &field : rfl::fields<ResponseType>()) {
          log(debug, " - {}: {}", field.name(), field.type());
        }
        stdexec::set_error(
            std::move(r),
            std::make_exception_ptr(std::runtime_error(
                "Failed to convert response from generic object")));
        return;
      }
      stdexec::set_value(std::move(r), response.value());
      return;
    };
    router_.registe(uuid_, std::move(callback));
  }
};

template <RequestType T, ResponseRouterType R> struct ResponseSender {
  R &router_;
  boost::uuids::uuid uuid_;

  using sender_concept = stdexec::sender_t;
  using completion_signatures = stdexec::completion_signatures<
      stdexec::set_value_t(typename T::Response),
      stdexec::set_error_t(std::exception_ptr), stdexec::set_stopped_t()>;

  template <stdexec::receiver Receiver> auto connect(Receiver r) noexcept {
    return ResponseOperation<Receiver, T, R>{std::move(r), router_, uuid_};
  }
};
}; // namespace Network
