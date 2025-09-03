#include "Logic/SignIn.hpp"
#include "Network/MessageType.hpp"

namespace Logic {

namespace Request {
struct SignIn {
  std::string id;
  std::string username;
  std::string password;
  using ResponseType = Network::Response::SignIn;
};

}; // namespace Request




}; // namespace Logic
