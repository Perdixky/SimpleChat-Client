#include "Async/Loop.hpp"
#include "TUI/screen.hpp"
#include <ftxui/component/loop.hpp>

using namespace boost;

auto main() -> int {
  auto end_point =
      asio::ip::tcp::endpoint(asio::ip::make_address("localhost"), 8080);
  std::shared_ptr<Network::Connection> conn =
      std::make_shared<Network::Connection>(end_point);

  TUI::Screen screen(conn);
  ftxui::Loop loop(TUI::Screen::getScreenRef(), screen.renderer());

  Async::Loop::startTimer([&loop] { loop.RunOnce(); },
                          std::chrono::milliseconds(16));

  Async::Loop::run();
  return 0;
}
