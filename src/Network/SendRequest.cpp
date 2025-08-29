#include "Network/Connection.hpp"
#include <rfl.hpp>
#include <rfl/msgpack.hpp>

template <Network::RequestType T>
auto Network::Connection::sendRequest(const T &request) -> T::ResponseType {
  using namespace boost;
  auto request_msg = rfl::msgpack::write(request);

  auto send =
      stream_.async_write_some(asio::buffer(request_msg), asioexec::use_sender);

}
