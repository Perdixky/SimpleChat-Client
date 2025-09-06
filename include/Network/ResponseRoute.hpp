#pragma once
#include "Network/ResponseError.hpp"
#include "Utils/Logger.hpp"

#include <boost/date_time.hpp>
#include <boost/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <chrono>
#include <expected>
#include <functional>
#include <rfl.hpp>
#include <rfl/Generic.hpp>
#include <unordered_map>

namespace Network {

enum class ResponseError;

class ResponseRouter {
public:
  using Callback_t = std::function<void(
      const std::expected<rfl::Generic::Object, ResponseError> &)>;

  auto registe(boost::uuids::uuid uuid, Callback_t &&callback) -> void {
    auto id = boost::uuids::to_string(uuid);
    LOG(trace) << "Registering callback for UUID: " << id;
    callbacks_.emplace(id, std::make_pair(std::chrono::steady_clock::now() +
                                              std::chrono::seconds(5),
                                          std::forward<Callback_t>(callback)));
  }

  auto route(const rfl::Generic::Object &generic) -> void {
    const auto id = generic.get("id").and_then(rfl::to_string).value();
    LOG(debug) << "Routing message with ID: " << id;
    auto it = callbacks_.find(id); // 这里一定有id
    if (it != callbacks_.end()) {
      it->second.second(generic);
      callbacks_.erase(it);
      LOG(trace) << "Callback found and executed for ID: " << id;
    } else {
      LOG(warning) << "No callback found for message with ID: " << id;
    }
  }

  auto timeoutCheck() -> void {
    auto now = std::chrono::time_point_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now());
    std::erase_if(callbacks_, [&](auto &item) {
      if (now > item.second.first) {
        LOG(info) << "Callback for ID " << item.first << " timed out.";
        item.second.second(
            std::unexpected<ResponseError>(ResponseError::TIMEOUT));
        return true;
      }
      return false;
    });
  }

  auto cancelAll(ResponseError &error) -> void {
    LOG(info) << "Cancelling all outstanding callbacks with error: "
              << rfl::enum_to_string(error);
    for (auto &[id, pair] : callbacks_) {
      pair.second(std::unexpected<ResponseError>(error));
    }
    callbacks_.clear();
  }

private:
  boost::uuids::random_generator generator_;
  std::unordered_map<
      std::string, std::pair<std::chrono::steady_clock::time_point, Callback_t>>
      callbacks_;
};
}; // namespace Network
