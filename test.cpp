#include "ftxui/dom/elements.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/loop.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/screen/screen.hpp>
#include <thread>
#include <vector>

using namespace ftxui;

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

auto loginMenu() -> Component {
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
      " Sign in ", [username, password] { /*screen.exit();*/ },
      ButtonOption::Animated(Color::Green));
  Component quit_button = Button(
      " Exit ", [] { /*screen.exit();*/ }, ButtonOption::Animated(Color::Red));

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

auto main() -> int {
  auto component = Renderer([] { return text("Hello, World!"); });
  auto root_ = Renderer([&] { return component->Render(); });
  auto component2 = Renderer([] { return text("Goodbye, World!"); });

  bool modal_active = false;
  int tab_selected = 0;
  auto login = loginMenu();
  auto tab = Container::Tab(
      {
          Renderer(login,
                   [&] {
                     if (modal_active) {
                       std::vector<Element> children = {
                           login->Render(),
                           vbox({hbox({filler(), text("im here") | border}),
                                 filler()})};
                       return dbox(children);
                     } else {
                       return login->Render();
                     }
                   }),
          Button({.label = "Tab2", .on_click = [&] {}}),
          Button({.label = "Tab3", .on_click = [&] {}}),
      },
      &tab_selected);

  auto screen = ScreenInteractive::FitComponent();
  std::optional<Loop> loop;
  loop.emplace(&screen, tab);

  std::jthread thread([&] {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    // tab_selected = 1;
    modal_active = true;
    screen.PostEvent(Event::Custom);
  });

  thread.detach();
  while (!loop->HasQuitted()) {
    loop->RunOnce();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}
