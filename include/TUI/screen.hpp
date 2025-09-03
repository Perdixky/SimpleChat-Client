#pragma once
#include "Network/Concepts.hpp"
#include "Network/Connection.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/screen/screen.hpp>

namespace TUI {

enum class ScreenState { Login, MainMenu, Settings };

template <Network::ConnectionType C> class Screen {
public:
  Screen(C &conn)
      : connection_(conn), screen_(ftxui::ScreenInteractive::Fullscreen()) {}

  // auto static getInstance(C &conn) -> Screen<C> & {
  //   static Screen screen(conn);
  //   return screen;
  // }

  auto changePage(const int page_number) -> void { page_number_ = page_number; }

  auto addMessage(const std::string &message) {
    using namespace ftxui;
    auto message_element = hbox({filler(), text(message) | border});
    message_elements_.push_back(message_element);
  }

  auto renderer() -> ftxui::Component {
    using namespace ftxui;
    return ftxui::Renderer(pages_, [&] {
      return dbox({
          vbox(message_elements_) | flex,
          pages_->Render() | flex,
      });
    });
  }

  auto getScreenRef() -> ftxui::ScreenInteractive * { return &screen_; }

  auto exit() -> void { screen_.Exit(); }

  auto loginMenu() -> ftxui::Component;

private:
  ftxui::ScreenInteractive screen_;
  int page_number_;
  ftxui::Component pages_;
  std::vector<ftxui::Element> message_elements_;
  C &connection_;
};

}; // namespace TUI
