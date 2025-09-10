#ifdef TrueColor
#undef TrueColor
#endif

#include "Async/Loop.hpp"
#include "GUI/Bind.hpp"
#include "Logic/Session.hpp"
// #include "TUI/screen.hpp"
#include "Utils/Logger.hpp"
#include "stdexec/__detail/__start_detached.hpp"
#include <exec/repeat_effect_until.hpp>
// #include <ftxui/component/loop.hpp>
#include <print>
#include <rfl.hpp>
#include <rfl/json.hpp>
#include <stdexec/execution.hpp>
#include <variant>
#include <webview/webview.h>

using namespace boost;

namespace {
constexpr unsigned char login_html[] = {
 #embed "../include/GUI/Login.html" suffix(, 0)
};
constexpr unsigned char chat_html[] = {
  #embed "../include/GUI/Chat.html" suffix(, 0)
};
};

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
  // stdexec::sync_wait(
  //     session.lowLevel().connect() | stdexec::let_value([&session]() {
  //       Async::Loop::submit(session.lowLevel().listen() |
  //                           exec::repeat_effect()); // 持续监听
  //       return session.echo("Hello, World!") | stdexec::then([](auto
  //       response) {
  //                std::print("Echo response: {}\n", response.message);
  //              }) |
  //              stdexec::upon_error([](const std::exception_ptr e_ptr) {
  //                if (e_ptr) {
  //                  try {
  //                    std::rethrow_exception(e_ptr);
  //                  } catch (const std::exception &e) {
  //                    std::print("Error in echo request: {}\n", e.what());
  //                  }
  //                }
  //              });
  //     }));

  stdexec::sync_wait(session.lowLevel().connect());
  Async::Loop::submit(session.lowLevel().listen() | exec::repeat_effect());

  webview::webview w(true, nullptr);
  w.set_title("Login");

  w.set_size(800, 600, WEBVIEW_HINT_NONE);
  w.set_html(reinterpret_cast<char const *>(login_html));
  LOG(debug) << "HTML content set in webview.";

  GUI::bind(w, "login",
            [&](const std::string &username, const std::string &password) {
              Logic::Data::SignIn credentials{username, password};
              return session.signIn(credentials);
            });

  GUI::bind(w, "log", [](const int severity, const std::string res){ 
    LOG(static_cast<SeverityLevel>(severity)) << res;
    return;
  });

  std::jthread async_thread([]() { Async::Loop::run(); });

  LOG(info) << "Starting GUI on main thread.";
  w.run();
  Async::Loop::stop();
  return 0;
}
