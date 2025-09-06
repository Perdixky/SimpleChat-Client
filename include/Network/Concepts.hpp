#pragma once
#include "Network/ResponseError.hpp"
#include <boost/uuid.hpp>
#include <concepts>
#include <expected>
#include <rfl/Generic.hpp>
#include <stdexec/execution.hpp>
#include <type_traits>

namespace Network {

namespace detail {

struct RequestConceptHelper {
  using ResponseType = int;
  std::string id;
};

}; // namespace detail

template <typename T>
concept ConnectionType = requires(T t, detail::RequestConceptHelper rc) {
  { t.connect() } -> stdexec::sender;
  { t.listen() } -> stdexec::sender;
  { t.sendRequest(rc) } -> stdexec::sender;
};

template <typename T>
concept RequestCallbackType =
    std::invocable<
        T, std::expected<rfl::Generic::Object, Network::ResponseError>> &&
    std::movable<T>;

template <typename T>
concept ResponseRouterType = requires {
  typename T::Callback_t;
} && requires(T &t, const boost::uuids::uuid &uuid, T::Callback_t &&callback) {
  {
    t.registe(uuid, std::forward<typename T::Callback_t>(callback))
  } -> std::same_as<void>;
  { t.route(rfl::Generic::Object{}) } -> std::same_as<void>;
};

}; // namespace Network
