#pragma once
#include "Network/Connection.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>

namespace TUI {

enum class ScreenState { Login, MainMenu, Settings };

class Screen {
public:
  Screen(std::shared_ptr<Network::Connection> conn) : connection_(conn) {}

  static auto changePage(const int page_number) -> void {
    page_number_ = page_number;
  }

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

  static auto getScreenRef() -> ftxui::ScreenInteractive * { return &screen_; }

  static auto exit() -> void { screen_.Exit(); }

  auto init() -> void {}

  auto loginMenu() -> ftxui::Component;

private:
  static ftxui::ScreenInteractive screen_;
  static int page_number_;
  ftxui::Component pages_;
  std::vector<ftxui::Element> message_elements_;
  std::shared_ptr<Network::Connection> connection_;
};

}; // namespace TUI

extern TUI::Screen screen;
