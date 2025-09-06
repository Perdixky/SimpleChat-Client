#include "Logic/LogicType.hpp"
#include "Network/Connection.hpp"
#include "Network/MessageType.hpp"
#include <boost/beast.hpp>
#include <boost/beast/websocket.hpp>

namespace Logic {
template <Network::ConnectionType C> class Session {
public:
  Session(C &&conn) : connection_(std::forward<C>(conn)) {};

  auto signIn(const std::string &username, const std::string &password)
      -> stdexec::sender auto {

    Request::SignIn request{.username = username, .password = password};

    auto sender = connection_.sendRequest(request);

    return sender;
  }

  auto heartbeat() -> stdexec::sender auto {
    Request::Heartbeat request{};

    auto sender = connection_.sendRequest(request);

    return sender;
  }

  auto echo(const std::string &message) -> stdexec::sender auto {
    Request::Echo request{.message = message};

    auto sender = connection_.sendRequest(request);

    return sender;
  }

  auto lowLevel() -> C & { return connection_; }

private:
  C connection_;
};
}; // namespace Logic
