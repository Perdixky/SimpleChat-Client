#pragma once
#include "rfl/Generic.hpp"
#include <boost/date_time.hpp>
#include <boost/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <chrono>
#include <functional>
#include <rfl.hpp>
#include <unordered_map>

namespace Network {
class ResponseRouter {
public:
  using callback_t = std::function<void()>;
  auto registe(boost::uuids::uuid uuid, callback_t &&callback) -> void {
    // auto uuid = boost::uuids::to_string(generator_());
    callbacks_.emplace(uuid,
                       std::make_pair(std::chrono::steady_clock::now(),
                                      std::forward<callback_t>(callback)));
  }

  auto route(rfl::Generic::Object &generic) -> void {
    auto it =
        callbacks_.find(generic.get("id").and_then(rfl::to_string).value());
    if (it != callbacks_.end()) {
      it->second.second();
      callbacks_.erase(it);
    } else {
      // TODO: 添加报错信息并考虑服务器异常
    }
  }

  auto timeout_check() -> void {
    auto now = std::chrono::time_point_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now());
    for (auto it = callbacks_.begin(); it != callbacks_.end();) {
      if (now - it->second.first > std::chrono::milliseconds(5000)) { // 5秒超时
        it = callbacks_.erase(it);
      } else {
        ++it;
      }
    }
  }

private:
  boost::uuids::random_generator generator_;
  std::unordered_map<
      std::string, std::pair<std::chrono::time_point<std::chrono::steady_clock,
                                                     std::chrono::milliseconds>,
                             std::function<void()>>>
      callbacks_;
};
}; // namespace Network
