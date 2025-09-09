#pragma once

#include <functional>
#include <string>
#include <tuple>
#include <vector>
#include <webview.h>

#include "Async/Loop.hpp"
#include "Utils/FunctionTraits.hpp"
#include "Utils/Logger.hpp"

#include <rfl/json.hpp>
#include <stdexec/execution.hpp>

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
    LOG(error) << "Failed to parse arguments from JSON: " << json_str;
    return std::nullopt;
  }
  return *result;
}

} // namespace detail

// 1. 异步 Bind: 用于返回 stdexec::sender 的函数
template <typename Func>
  requires stdexec::sender<
      typename Utils::function_traits<std::decay_t<Func>>::return_type>
void bind(webview::webview &w, const std::string &name, Func &&func) {
  using traits = Utils::function_traits<std::decay_t<Func>>;
  using args_tuple = typename traits::decayed_args_as_tuple;

  w.bind(
      name,
      [name = std::string(name), f = std::forward<Func>(func),
       &w](const std::string &id, const std::string &req, void *) {
        LOG(trace) << "Async call received for '" << name
                   << "' with id: " << id;
        auto args = detail::parse_args<args_tuple>(req);

        if (!args) [[unlikely]] {
          // 参数解析失败，立即返回错误
          auto error_tuple = std::make_tuple("Invalid arguments");
          w.resolve(id, 1, rfl::json::write(error_tuple));
          return;
        }
        auto sender =
            std::apply(f, std::move(*args)) |
            stdexec::then([&w, id = id](auto &&...values) {
              // 将异步操作的结果打包成一个元组，然后序列化
              auto result_tuple = std::make_tuple(values...);
              w.resolve(id, 0, rfl::json::write(result_tuple));
            }) |
            stdexec::upon_error([&w, id = id](auto &&...errors) {
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
              w.resolve(id, 1, rfl::json::write(error_tuple));
            });

        Async::Loop::submit(std::move(sender));
      },
      nullptr);
}

// 2. 同步 Bind: 用于返回普通值或void的函数
template <typename Func>
  requires(!stdexec::sender<
           typename Utils::function_traits<std::decay_t<Func>>::return_type>)
void bind(webview::webview &w, const std::string &name, Func &&func) {
  using traits = Utils::function_traits<std::decay_t<Func>>;
  using args_tuple = typename traits::decayed_args_as_tuple;
  using return_type = typename traits::return_type;

  w.bind(name, [name = std::string(name),
                f = std::forward<Func>(func)](const std::string &req) {
    LOG(trace) << "Sync call received for '" << name << "'";
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

} // namespace GUI
