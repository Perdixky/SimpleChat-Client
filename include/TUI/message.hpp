#include <string>
#include <map>

enum class MessageType {
  Login,
};

namespace TUI {
  using Message = std::pair<MessageType, std::string>;
};
