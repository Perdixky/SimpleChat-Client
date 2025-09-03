#include <memory>
#include <string>
#include <vector>

#include "ftxui/component/component.hpp"
#include "ftxui/dom/elements.hpp"

#include "TUI/screen.hpp"

using namespace ftxui;

// 函数：创建 ASCII Art Logo (已修正)
auto CreateLogo() -> Element {
  auto linear_gradient =
      LinearGradient().Angle(180).Stop(Color::Red).Stop(Color::Blue);
  return vbox({
             text("   ██████╗ ██╗███╗   ███╗██████╗ ██╗     ") | center,
             text("  ██╔════╝ ██║████╗ ████║██╔══██╗██║     ") | center,
             text("  ╚█████╗  ██║██╔████╔██║██████╔╝██║     ") | center,
             text("   ╚═══██╗ ██║██║╚██╔╝██║██╔═══╝ ██║     ") | center,
             text("  ██████╔╝ ██║██║ ╚═╝ ██║██║     ███████╗") | center,
             text("  ╚═════╝  ╚═╝╚═╝     ╚═╝╚═╝     ╚══════╝") | center,
             text(""),
             text("  ██████╗██╗ ██╗ █████╗ ████████╗") | center,
             text(" ██╔════╝██║ ██║██╔══██╗╚══██╔══╝") | center,
             text("██║     ██████║███████║   ██║ ") | center,
             text("██║     ██╔═██║██╔══██║   ██║ ") | center,
             text("╚██████╗██║ ██║██║  ██║   ██║ ") | center,
             text(" ╚═════╝╚═╝ ╚═╝╚═╝  ╚═╝   ╚═╝ ") | center,
         }) |
         center | color(linear_gradient);
}

template <Network::ConnectionType C>
auto TUI::Screen<C>::loginMenu() -> Component {
  // --- 1. 定义状态变量 ---
  // std::string username;
  // std::string password;
  std::shared_ptr<std::string> username = std::make_shared<std::string>();
  std::shared_ptr<std::string> password = std::make_shared<std::string>();

  // --- 2. 创建组件 ---
  InputOption username_option;
  username_option.placeholder = "Enter your username";

  InputOption password_option;
  password_option.password = true; // 将输入显示为 '*'
  password_option.placeholder = "Enter your password";

  Component username_input = Input(username.get(), username_option);
  Component password_input = Input(password.get(), password_option);

  Component login_button = Button(
      " Sign in ", [username, password, this] {  },
      ButtonOption::Animated(Color::Green));
  Component quit_button = Button(
      " Exit ", [this] { exit(); }, ButtonOption::Animated(Color::Red));

  // --- 3. 组织组件布局 ---
  auto component = Container::Vertical({
      username_input,
      password_input,
      Container::Horizontal({
          login_button,
          quit_button,
      }),
  });

  // --- 4. 定义渲染器 ---
  return Renderer(
      component, [username_input, password_input, login_button, quit_button]() {
        return vbox({
                   CreateLogo(),
                   separator(),
                   gridbox({
                       {text("   Username: ") | bold | color(Color::White),
                        username_input->Render() | size(WIDTH, EQUAL, 30)},
                       {text("   Password: ") | bold | color(Color::White),
                        password_input->Render() | size(WIDTH, EQUAL, 30)},
                   }) | center,
                   separator(),
                   hbox({
                       filler(),
                       login_button->Render(),
                       text(" "),
                       quit_button->Render(),
                       filler(),
                   }),
               }) |
               borderRounded | center;
      });
}
