#include "Network/Connection.hpp"
#include <rfl.hpp>
#include <rfl/msgpack.hpp>

template <Network::RequestType T>
auto Network::Connection::sendRequest(const T &request) -> stdexec::sender auto {
  using namespace boost;
  auto request_msg = rfl::msgpack::write(request);

  static boost::uuids::random_generator
      generator; // Added static to avoid re-initialization

  auto uuid = generator();

  request_msg["id"] = boost::uuids::to_string(uuid);

  auto send = stream_.async_write_some(asio::buffer(request_msg),
                                       asioexec::use_sender) |
              stdexec::then([uuid](auto) { return uuid; });
}
