#pragma once
#include "TUI/login.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>

namespace TUI {

enum class ScreenState { Login, MainMenu, Settings };

class Screen {
public:
  static auto changePage(const int page_number) -> void {}

  static auto addMessage(const std::string &message) {
    using namespace ftxui;
    auto message_element = hbox({filler(), text(message) | border});
    message_elements_.push_back(message_element);
  }

  static auto Renderer() -> ftxui::Component {
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

private:
  static ftxui::ScreenInteractive screen_;
  static int page_number_;
  static ftxui::Component pages_;
  static std::vector<ftxui::Element> message_elements_;
};

}; // namespace TUI

extern TUI::Screen screen;
