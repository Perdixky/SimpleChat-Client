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
  auto registe(const std::function<void()> &&callback) -> std::string {
    auto uuid = boost::uuids::to_string(generator_());
    callbacks_.emplace(uuid, std::move(callback));
    return uuid;
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

private:
  boost::uuids::random_generator generator_;
  std::unordered_map<
      std::string, std::pair<std::chrono::time_point<std::chrono::steady_clock,
                                                     std::chrono::milliseconds>,
                             std::function<void()>>>
      callbacks_;
};
}; // namespace Network
