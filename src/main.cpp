#include "Async/Loop.hpp"
#include "TUI/screen.hpp"
#include <exec/repeat_effect_until.hpp>
#include <ftxui/component/loop.hpp>

using namespace boost;

// 所有代码都在main函数中执行，确保所有对象在main函数中存活
auto main() -> int {
  auto end_point =
      asio::ip::tcp::endpoint(asio::ip::make_address("localhost"), 8080);
  Network::ResponseRouter<
      decltype([](std::expected<rfl::Generic::Object, Network::ResponseError>) {
      })>
      router;
  Network::Connection<decltype(router)> conn(std::move(end_point), router);

  auto screen = TUI::Screen(conn);
  ftxui::Loop loop(screen.getScreenRef(), screen.renderer());
  Async::Loop::startTimer([&loop] { loop.RunOnce(); },
                          std::chrono::milliseconds(16));

  Async::Loop::submit(conn.listen() | exec::repeat_effect()); // 持续监听
  Async::Loop::run();
  return 0;
}
