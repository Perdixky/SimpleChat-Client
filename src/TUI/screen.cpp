#include "TUI/screen.hpp"
#include "TUI/login.hpp"

#include <boost/asio/detail/chrono.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/system/detail/error_code.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/node.hpp>

using namespace ftxui;

enum class ScreenState { Login, MainMenu, Settings, Exit };

using namespace ftxui;
using namespace boost;
namespace TUI {

ScreenInteractive Screen::screen_ = ScreenInteractive::Fullscreen();
int Screen::page_number_ = 0;
Component Screen::pages_ = Container::Tab({loginMenu()}, &page_number_);
std::vector<ftxui::Element> Screen::message_elements_;

}; // namespace TUI
TUI::Screen screen;
