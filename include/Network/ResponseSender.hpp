#pragma once
#include "Network/MessageType.hpp"
#include "Network/ResponseRoute.hpp"
#include "rfl/Generic.hpp"
#include "rfl/from_generic.hpp"
#include <boost/uuid/uuid.hpp>
#include <expected>
#include <rfl.hpp>
#include <stdexec/execution.hpp>

namespace Network {

template <stdexec::receiver Receiver, RequestType T, ResponseRouterType R>
struct ResponseOperation {
  using ResponseType = T::ResponseType;

  Receiver receiver_;

  R& router_;

  boost::uuids::uuid uuid_;

  auto start() noexcept -> void {
    auto callback = [this, r = std::move(receiver_)](
                        std::expected<rfl::Generic::Object, ResponseError>
                            &result) mutable {
      if (!result) {
        stdexec::set_error(
            std::move(r),
            std::make_exception_ptr(std::runtime_error("Request error")));
        // TODO: 这里可以根据不同的错误类型传递不同的异常
        return;
      }

      auto response = result.value(); // 这里一定有值

      if constexpr (requires { ResponseType::error; }) {
        if (auto err = response.get("error"); err) {
          stdexec::set_error(std::move(r),
                             std::make_exception_ptr(
                                 std::runtime_error(err->to_string().value())));
          return;
        }
      }

      if constexpr (requires { ResponseType::content; }) {
        if (auto res = response.get("result"); res) {
          ResponseType response = rfl::from_generic<ResponseType>(response);
          stdexec::set_value(std::move(r), response);
          return;
        } else {
          stdexec::set_error(
              std::move(r),
              std::make_exception_ptr(std::runtime_error("No result")));
          return;
        }
      } else {
        ResponseType response = rfl::from_generic<ResponseType>(response);
        stdexec::set_value(std::move(r), response);
        return;
      }
    };
    router_.get().registe(uuid_, std::move(callback));
  }
};

template <RequestType T, ResponseRouterType R> struct ResponseSender {
  R& router_;
  boost::uuids::uuid uuid_;

  using sender_concept = stdexec::sender_t;
  using completion_signatures = stdexec::completion_signatures<
      stdexec::set_value_t(typename T::ResponseType),
      stdexec::set_error_t(std::exception_ptr), stdexec::set_stopped_t()>;

  template <stdexec::receiver Receiver> auto connect(Receiver r) noexcept {
    return ResponseOperation<Receiver, T, R>{std::move(r), router_, uuid_};
  }
};
}; // namespace Network
