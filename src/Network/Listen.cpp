#include <Network/Connection.hpp>
#include <rfl/msgpack.hpp>

auto Network::Connection::listen() -> void {
  using namespace boost;
  using namespace stdexec;
  char buffer[4096];
  auto sender =
      stream_.async_read_some(asio::buffer(buffer), asioexec::use_sender) |
      then([&buffer, this](auto lenth) {
      auto generic = rfl::msgpack::read<rfl::Generic::Object>(std::span(buffer, lenth));
      auto id = generic.get("id").and_then(rfl::to_string);
      if (id) {
        router_.route(generic);
      }
      else {
        // TODO: 处理没有 ID 的消息
      }
    });
}
