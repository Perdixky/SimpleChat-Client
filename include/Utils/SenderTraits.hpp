#include <stdexec/execution.hpp>

namespace Utils {

// --- 自定义的类型萃取工具 ---
template <typename T> struct nested_type_from_variant_tuple;
// 为 std::variant<std::tuple<...>> 提供特化版本
template <typename InnerType>
struct nested_type_from_variant_tuple<std::variant<std::tuple<InnerType>>> {
  using type = InnerType;
};

template <typename T> struct sender_completion_type {
  using VariantType = stdexec::value_types_of_t<T, stdexec::env<>>;
  using type = typename nested_type_from_variant_tuple<VariantType>::type;
};

template <typename T>
using sender_completion_t = typename sender_completion_type<T>::type;
}; // namespace Utils
