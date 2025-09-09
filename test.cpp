#include "rfl/Generic.hpp"
#include <Logic/LogicType.hpp>
#include <rfl.hpp>
#include <rfl/json.hpp>
#include <webview.h>
#include <print>

constexpr unsigned char const html[] = {
#embed "include/GUI/Login.html" suffix(, 0)
};
auto main() -> int {
  webview::webview w(true, nullptr);
  w.set_title("Login");
  w.set_size(800, 600, WEBVIEW_HINT_NONE);
  auto h = reinterpret_cast<const char *>(html);
  w.bind("login", [](const std::string_view req) {
    auto result = rfl::json::read<std::vector<Logic::Data::SignIn>>(req);
    if (result) {
      auto object = (*result)[0];
      std::print("Received login request: username={}, password={}\n", object.username, object.password);
      std::print("Original JSON: {}\n", req);
      if (object.username == "user" && object.password == "pass") {
        Logic::Response::SignIn res{.success = true};
        return rfl::json::write(res);
      } else {
        Logic::Response::SignIn res{.success = false};
        return rfl::json::write(res);
      }
    }
    else {
      std::print("Failed to parse JSON: {}\n JSON string: {}\n", result.error().what(), req);
    }
  });
  w.set_html(h);
  w.run();
  auto x = w.eval("");
  auto y = w.resolve("", 0, "");
  return 0;
}
