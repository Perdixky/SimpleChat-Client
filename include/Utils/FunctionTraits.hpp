#include <functional> // For std::invoke_result_t
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

namespace Utils {
template <typename T> struct function_traits;

// 2. 针对普通函数指针的特化
template <typename ReturnType, typename... Args>
struct function_traits<ReturnType (*)(Args...)> {
  using return_type = ReturnType;
  static constexpr size_t arity = sizeof...(Args);

  // 使用 std::tuple 来方便地访问参数类型
  using args_as_tuple = std::tuple<Args...>;

  // 提取第一个参数类型
  using first_arg_type = std::tuple_element_t<0, args_as_tuple>;
};

// 3. 针对 lambda 和函数对象 (Functor) 的特化
//    我们通过萃取其 operator() 的类型来工作
template <typename Functor>
struct function_traits : function_traits<decltype(&Functor::operator())> {};

// 4. 针对 const 成员函数指针的特化 (lambda 的 operator() 通常是 const)
template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType (ClassType::*)(Args...) const>
    : function_traits<ReturnType (*)(Args...)> {
}; // 继承自函数指针版本，复用代码

// 5. 针对非 const 成员函数指针的特化 (用于 mutable lambda)
template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType (ClassType::*)(Args...)>
    : function_traits<ReturnType (*)(Args...)> {}; // 同上

// 定义一个方便的别名模板
template <typename F>
using first_arg_t = typename function_traits<F>::first_arg_type;
}; // namespace Utils
