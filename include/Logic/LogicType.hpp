#include "Network/MessageType.hpp"

namespace Logic {

namespace Response {
struct SignIn {
  bool success;
  std::string message; // 例如错误信息
};

}; // namespace Response
namespace Request {

struct SignIn {
  using ResponseType = Response::SignIn;
  std::string id;

  std::string username;
  std::string password;
};

}; // namespace Request

}; // namespace Logic
