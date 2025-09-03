#include <concepts>

namespace Utils {

// template <typename F, typename... Args>
// concept MoveOnlyFunc =
//     std::invocable<F, Args...> && // F 可以用 Args... 类型的参数调用
//     std::movable<F> &&            // F 是可移动的
//     !std::copyable<F>;            // F 是不可复制的

}; // namespace Utils
