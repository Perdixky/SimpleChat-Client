#pragma once

#include "Logic/Session.hpp"
#include "Network/Connection.hpp"
#include "Utils/Logger.hpp"
#include <webview/webview.h>

#include "Async/Loop.hpp"
#include "Utils/FunctionTraits.hpp"
#include <functional>
#include <rfl/json.hpp>
#include <stdexec/execution.hpp>
#include <string>
#include <tuple>
#include <vector>

namespace {
constexpr unsigned char html[] = {
#embed "index.html" suffix(, 0)
};

template <std::size_t size> auto consteval makeFormatString() -> std::string {
  std::string fmt = "{}()";
  for (std::size_t i = 0; i < size - 1; ++i) {
    fmt += "{}";
    if (i != size - 2)
      fmt += ", ";
  }
  fmt += ")";
  return fmt;
}

auto consteval preprocessArg(auto arg) {
  if constexpr (std::is_convertible_v<decltype(arg), std::string>) {
    return "\"" + arg + "\"";
  } else if constexpr (std::is_same_v<decltype(arg), bool>) {
    return arg ? "true" : "false";
  } else {
    return arg;
  }
}

}; // namespace

namespace GUI {

namespace detail {

// 帮助函数，用于从JSON字符串反序列化为参数元组
template <typename TupleType>
auto parse_args(const std::string &json_str) -> std::optional<TupleType> {
  if constexpr (std::tuple_size_v<TupleType> == 0) {
    return TupleType{};
  }
  auto result = rfl::json::read<TupleType>(json_str);
  if (!result) {
    log(error, "Failed to parse arguments from JSON: {}", json_str);
    return std::nullopt;
  }
  return *result;
}

} // namespace detail

template <Network::ConnectionType C> class GUI {
public:
  GUI(Logic::Session<C> &session) : session_(session), view_(true, nullptr) {
    log(info, "Initializing GUI...");
    view_.set_title("Login");
    view_.set_size(800, 600, WEBVIEW_HINT_NONE);
    view_.set_html(reinterpret_cast<char const *>(html));
    log(debug, "HTML content set in webview.");
    bindCallbacks();
  }

  auto eval(const std::string &func, auto &&...args) -> void {
    std::string format_string = makeFormatString<sizeof...(args)>();
    std::string js_string =
        std::format(format_string, func, preprocessArg(args)...);
    view_.eval(js_string);
  }

  auto run() -> void {
    log(info, "Starting GUI on main thread.");
    view_.run();
  }

private:
  // 1. 异步 Bind: 用于返回 stdexec::sender 的函数
  template <typename Func>
    requires stdexec::sender<
        typename Utils::function_traits<std::decay_t<Func>>::return_type>
  void bind(const std::string &name, Func &&func) {
    using traits = Utils::function_traits<std::decay_t<Func>>;
    using args_tuple = typename traits::decayed_args_as_tuple;

    view_.bind(
        name,
        [this, name = std::string(name), f = std::forward<Func>(func)](
            const std::string &id, const std::string &req, void *) {
          log(trace, "Async call received for '{}' with id: {}, request: {}",
              name, id, req);
          auto args = detail::parse_args<args_tuple>(req);

          if (!args) [[unlikely]] {
            // 参数解析失败，立即返回错误
            auto error_tuple = std::make_tuple("Invalid arguments");
            this->view_.resolve(id, 1, rfl::json::write(error_tuple));
            return;
          }
          auto sender =
              std::apply(f, std::move(*args)) |
              stdexec::then([this, id = id](auto &&...values) {
                // 将异步操作的结果打包成一个元组，然后序列化
                auto result_tuple = std::make_tuple(values...);
                this->view_.resolve(id, 0, rfl::json::write(result_tuple));
              }) |
              stdexec::upon_error([this, id = id](auto &&...errors) {
                // 同样地，处理错误情况
                std::string error_message = "Unknown error";
                // We'll just process the first error in the pack.
                // stdexec typically sends a single exception_ptr.
                if constexpr (sizeof...(errors) > 0) {
                  auto first_error =
                      std::get<0>(std::forward_as_tuple(errors...));
                  if (first_error) { // Check if the exception_ptr is not null
                    try {
                      std::rethrow_exception(first_error);
                    } catch (const std::exception &e) {
                      error_message = e.what();
                    } catch (...) {
                      error_message =
                          "An unknown non-standard exception occurred.";
                    }
                  }
                }
                auto error_tuple = std::make_tuple(error_message);
                this->view_.resolve(id, 1, rfl::json::write(error_tuple));
              });

          Async::Loop::submit(std::move(sender));
        },
        nullptr);
  }

  // 2. 同步 Bind: 用于返回普通值或void的函数
  template <typename Func>
    requires(!stdexec::sender<
             typename Utils::function_traits<std::decay_t<Func>>::return_type>)
  void bind(const std::string &name, Func &&func) {
    using traits = Utils::function_traits<std::decay_t<Func>>;
    using args_tuple = typename traits::decayed_args_as_tuple;
    using return_type = typename traits::return_type;

    view_.bind(name, [name = std::string(name),
                      f = std::forward<Func>(func)](const std::string &req) {
      log(trace, "Sync call received for '{}' with request: {}", name, req);
      auto args = detail::parse_args<args_tuple>(req);
      if (!args) [[unlikely]] {
        // 参数解析失败，立即返回错误
        auto error_tuple = std::make_tuple("Invalid arguments");
        return rfl::json::write(error_tuple);
      }

      if constexpr (std::is_void_v<return_type>) {
        std::apply(f, std::move(*args));
        return std::string{};
      } else {
        auto result = std::apply(f, std::move(*args));
        return rfl::json::write(result);
      }
    });
  }

  auto bindCallbacks() -> void {
    // Unify JS <-> C++ binding names to match Logic::Request::method_name
    bind("SignIn", [&](const std::string &email, const std::string &password) {
      return session_.SignIn(email, password);
    });
    bind("log", [](const int severity, const std::string res) {
      log(static_cast<SeverityLevel>(severity), "{}", res);
      return;
    });
    bind("SignUp",
         [this](const std::string username, const std::string &invite_code,
                const std::string &email, const std::string &verification_code,
                const std::string &password) {
           // verification_code is currently unused by backend
           return this->session_.SignUp(username, invite_code, email,
                                        verification_code, password);
         });
    bind("GetConversationList", [this]() {
      log(debug, "Fetching conversation list...");
      return this->session_.GetConversationList();
    });
    bind("GetMessageHistory", [this](int conversation_id) {
      log(debug, "Fetching message history for conversation ID: {}",
          conversation_id);
      return this->session_.GetMessageHistory(conversation_id);
    });
    bind("GetConversationMemberList", [this](int conversation_id) {
      log(debug, "Fetching members for conversation ID: {}", conversation_id);
      return this->session_.GetConversationMemberList(conversation_id);
    });
    bind("AddFriend", [this](int friend_id) {
      return this->session_.AddFriend(friend_id);
    });
    bind("FindUsers", [this](const std::string &query) {
      return this->session_.FindUsers(query);
    });
  }

  Logic::Session<C> &session_;
  webview::webview view_;
};

} // namespace GUI
