#include "Network/MessageType.hpp"
#include <chrono>

namespace Logic {

namespace Response {
struct SignIn {
  bool success;
  // std::string message; // 例如错误信息
};

struct Heartbeat {
  // 服务器可以返回一些状态信息
  std::optional<std::string> error;
  unsigned long long serverTimestamp;
};

struct Echo {
  std::string message;
};

}; // namespace Response
namespace Request {

struct SignIn {
  using ResponseType = Response::SignIn;
  std::string id;

  std::string method = "SignIn";

  std::string username;
  std::string password;
};

struct Heartbeat {
  using ResponseType = Response::Heartbeat;
  std::string id;

  std::string method = "Heartbeat";
  unsigned long long timestamp =
      std::chrono::steady_clock::now().time_since_epoch().count();
};

struct Echo {
  using ResponseType = Response::Echo;
  std::string id;

  std::string method = "Echo";
  std::string message;
};

}; // namespace Request

}; // namespace Logic
