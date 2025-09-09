#include <rfl.hpp>
#include <rfl/json.hpp>
#include <stdexec/execution.hpp>
#include <string_view>
#include "Utils/FunctionTraits.hpp"
#include "Utils/Logger.hpp"

namespace GUI {

// Synchronous callback builder
auto callback(auto func, std::string_view req) -> std::string {
  using ArgType = Utils::first_arg_t<decltype(func)>;
  using RetType = std::invoke_result_t<decltype(func), ArgType>;

  auto result = rfl::json::read<std::vector<ArgType>>(req);
  if (result) {
    auto object = (*result)[0];
    auto ret = func(object);
    LOG(trace) << "Callback function executed successfully.";
    return rfl::json::write(static_cast<RetType>(ret));
  } else {
    // 处理解析错误
    LOG(error) << "Failed to parse JSON: " << result.error().what() << " JSON string: " << req;
    return ""; // TODO: 返回适当的错误信息
  }
}

// Asynchronous callback builder
auto callback(auto async_func, std::string_view id, std::string_view req) -> void {
  using ArgType = Utils::first_arg_t<decltype(async_func)>;
  auto result = rfl::json::read<std::vector<ArgType>>(req);
  if (result) {
    auto object = (*result)[0];
    async_func(object) | stdexec::then([](auto result){
      
    });
    LOG(trace) << "Async callback function executed successfully.";
  } else {
    // 处理解析错误
    LOG(error) << "Failed to parse JSON: " << result.error().what() << " JSON string: " << req;
  }
}

};
