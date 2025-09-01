#include "Network/ResponseRoute.hpp"
#include "ctre.hpp"
#include "rfl/Generic.hpp"
#include "rfl/from_generic.hpp"
#include <Network/MessageType.hpp>
#include <boost/uuid/uuid.hpp>
#include <rfl.hpp>
#include <stdexec/execution.hpp>

template <stdexec::receiver Receiver, Network::RequestType T>
struct ResponseOperation {
  using ResponseType = T::ResponseType;

  Receiver receiver_;

  boost::uuids::uuid uuid_;

  auto tag_invoke(stdexec::start_t, ResponseOperation &self) noexcept -> void {
    auto callback =
        [r = std::move(self.receiver_)](rfl::Generic::Object &generic) mutable {
          ResponseType response = rfl::from_generic<ResponseType>(generic);
          stdexec::set_value(std::move(r), response);
        };
  }
};

template <Network::RequestType T> struct ResponseSender {

  template <stdexec::receiver Receiver>
  auto tag_invoke(stdexec::connect_t, ResponseSender, Receiver r) noexcept {
    return ResponseOperation<Receiver, T>{std::move(r)};
  }
};
