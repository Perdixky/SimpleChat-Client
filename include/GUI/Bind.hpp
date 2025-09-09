#include <rfl.hpp>
#include <rfl/json.hpp>
#include <stdexec/execution.hpp>
#include <webview.h>
#include <Utils/Logger.hpp>
#include <Utils/FunctionTraits.hpp>
#include <Utils/SenderTraits.hpp>
#include <Async/Loop.hpp>

namespace GUI {

template <typename Func>
auto bind(webview::webview &w, std::string name, Func f) {
  using ArgType = Utils::first_arg_t<decltype(f)>;
  w.bind(
      name,
      [&](std::string_view id, std::string_view req, void *) {
        auto result = rfl::json::read<std::vector<ArgType>>(req);
        if (result) {
          auto object = (*result)[0];
          LOG(debug) << "Original JSON: " << req;
                    stdexec::sender auto sender =
              f(object) |
              stdexec::then([&w, id = std::string(id)](auto res) {
                static_assert(std::is_same_v<decltype(res), Utils::sender_completion_t<decltype(f(object))>>);
                w.resolve(id, 0, rfl::json::write(res));
              });
          Async::Loop::submit(std::move(sender));
        }
      },
      nullptr);
}

};
