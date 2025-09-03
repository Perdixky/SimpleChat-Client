#pragma once
#include <concepts>
#include <string>

namespace Network {

////////////////////////////////////////////////////////
template <typename T>
concept MessageType = requires {
  T::source;
  T::level;
  T::timestamp;
  T::content;
};
namespace Message {
struct DefaultMessage {
  std::string source;
  std::string level;
  std::string timestamp;
  std::string content;
};
}; // namespace Message

////////////////////////////////////////////////////////
namespace Response {

struct SignIn {
  bool success;
  // std::optional<std::string>
}; // namespace Response

}; // namespace Response

////////////////////////////////////////////////////////
template <typename T>
concept RequestType = requires {
  typename T::ResponseType;
  T::id;
};

}; // namespace Network
