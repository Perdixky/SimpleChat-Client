#pragma once
#include "Network/ResponseError.hpp"
#include "Utils/Function.hpp"

#include <boost/date_time.hpp>
#include <boost/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <chrono>
#include <expected>
#include <rfl.hpp>
#include <rfl/Generic.hpp>

namespace Network {

enum class ResponseError;

template <typename callback_t>
  requires(
      std::is_invocable_v<callback_t,
                          std::expected<rfl::Generic::Object, ResponseError>> &&
      std::movable<callback_t>)
class ResponseRouter {
public:
  using Callback_t = callback_t;

  auto registe(boost::uuids::uuid uuid, callback_t &&callback) -> void {
    // auto uuid = boost::uuids::to_string(generator_());
    callbacks_.emplace(uuid,
                       std::make_pair(std::chrono::steady_clock::now() +
                                          std::chrono::seconds(5),
                                      std::forward<callback_t>(callback)));
  }

  auto route(const rfl::Generic::Object &generic) -> void {
    auto it = callbacks_.find(
        generic.get("id").and_then(rfl::to_string).value()); // 这里一定有id
    if (it != callbacks_.end()) {
      it->second.second(generic);
      callbacks_.erase(it);
    } else {
      // TODO: 添加报错信息并考虑服务器异常
    }
  }

  auto timeoutCheck() -> void {
    auto now = std::chrono::time_point_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now());
    callbacks_.erase_if([&](auto &it) {
      if (now > it->second.first) {
        it->second.second(
            std::unexpected<ResponseError>(ResponseError::TIMEOUT));
        return true;
      }
      return false;
    });
  }

  auto cancelAll(ResponseError &error) -> void {
    for (auto &[id, pair] : callbacks_) {
      pair.second(
          std::unexpected<ResponseError>(error)); // 这里可以考虑传递错误信息
    }
    callbacks_.clear();
  }

private:
  boost::uuids::random_generator generator_;
  std::unordered_map<
      std::string, std::pair<std::chrono::steady_clock::time_point, callback_t>>
      callbacks_;
};
}; // namespace Network
