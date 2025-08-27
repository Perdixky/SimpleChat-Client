#include "Async/EventLoop.hpp"
#include "TUI/screen.hpp"
#include <ftxui/component/loop.hpp>

auto main() -> int {
  ftxui::Loop loop(TUI::Screen::getScreenRef(), TUI::Screen::Renderer());
  Async::loop.startTimer([&loop] { loop.RunOnce(); },
                         std::chrono::milliseconds(16));

  loop.Run();
  return 0;
}
