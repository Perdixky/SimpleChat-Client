#include "Async/Loop.hpp"
#include "GUI/GUI.hpp"
#include "Logic/Session.hpp"
#include "Utils/Logger.hpp"
#include <exec/repeat_effect_until.hpp>
#include <stdexec/execution.hpp>

using namespace boost;

// 所有代码都在main函数中执行，确保所有对象在main函数中存活
auto main() -> int {
  initLogging();
  log(info, "Application starting...");
  auto end_point =
      asio::ip::tcp::endpoint(asio::ip::make_address("127.0.0.1"), 8888);
  Network::ResponseRouter router;
  Network::Connection conn(std::move(end_point), router);

  Logic::Session<decltype(conn)> session(std::move(conn));

  stdexec::sync_wait(session.lowLevel().connect());
  Async::Loop::submit(session.alive_listen());

  GUI::GUI gui(session);

  std::jthread async_thread([]() { Async::Loop::run(); });

  gui.run();
  Async::Loop::stop();
  return 0;
}
