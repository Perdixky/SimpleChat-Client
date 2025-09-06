#include "Async/Loop.hpp"
#include "Logic/Session.hpp"
#include "TUI/screen.hpp"
#include "Utils/Logger.hpp"
#include "stdexec/__detail/__start_detached.hpp"
#include <exec/repeat_effect_until.hpp>
#include <ftxui/component/loop.hpp>
#include <print>
#include <stdexec/execution.hpp>
#include <variant>

using namespace boost;

// 所有代码都在main函数中执行，确保所有对象在main函数中存活
auto main() -> int {
  initLogging();
  LOG(info) << "Application starting...";
  auto end_point =
      asio::ip::tcp::endpoint(asio::ip::make_address("127.0.0.1"), 8080);
  Network::ResponseRouter router;
  Network::Connection conn(std::move(end_point), router);

  // auto screen = TUI::Screen();
  // ftxui::Loop loop(screen.getScreenRef(), screen.renderer());
  // Async::Loop::startTimer([&loop] { loop.RunOnce(); },
  //                         std::chrono::milliseconds(16));
  //

  Logic::Session session(std::move(conn));
  stdexec::sync_wait(
      session.lowLevel().connect() | stdexec::let_value([&session]() {
        Async::Loop::submit(session.lowLevel().listen() |
                            exec::repeat_effect()); // 持续监听
        return session.echo("Hello, World!") | stdexec::then([](auto
        response) {
                 std::print("Echo response: {}\n", response.message);
               }) |
               stdexec::upon_error([](const std::exception_ptr e_ptr) {
                 if (e_ptr) {
                   try {
                     std::rethrow_exception(e_ptr);
                   } catch (const std::exception &e) {
                     std::print("Error in echo request: {}\n", e.what());
                   }
                 }
               });
      }));

  // Async::Loop::run();
  return 0;
}
