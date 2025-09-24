#pragma once

#include "Logic/LogicType.hpp"
#include "Network/Connection.hpp"
#include "Network/MessageType.hpp"
#include <boost/beast.hpp>
#include <boost/beast/websocket.hpp>
#include <cstdint>
#include <string>

namespace Logic {
template <Network::ConnectionType C> class Session {
public:
  Session(C &&conn) : connection_(std::forward<C>(conn)) {};

  // API methods are renamed to match Logic::Request::<Name>::method_name
  auto SignIn(const std::string &email, const std::string password)
      -> stdexec::sender auto {

    Request::SignIn request{.email = email, .password = password};

    auto sender = connection_.sendRequest(request);

    return sender;
  }

  auto SignUp(const std::string &username, const std::string &invitation_code,
              const std::string &email, const std::string &verification_code,
              const std::string &password) -> stdexec::sender auto {

    Request::SignUp request{.username = username,
                            .invite_code = invitation_code,
                            .email = email,
                            // .verification_code = verification_code,
                            .password = password};

    auto sender = connection_.sendRequest(request);

    return sender;
  }

  auto GetConversationList() -> stdexec::sender auto {
    Request::GetConversationList request{};

    auto sender = connection_.sendRequest(request);

    return sender;
  }

  auto GetConversationMemberList(const int conversation_id)
      -> stdexec::sender auto {
    Request::GetConversationMemberList request{
        .conversation_id = static_cast<uint64_t>(conversation_id)};

    auto sender = connection_.sendRequest(request);

    return sender;
  }

  auto GetMessageHistory(const int conversation_id) -> stdexec::sender auto {
    Request::GetMessageHistory request{
        .conversation_id = static_cast<uint64_t>(conversation_id)};

    auto sender = connection_.sendRequest(request);

    return sender;
  }

  auto AddFriend(const int friend_id) -> stdexec::sender auto {
    Request::AddFriend request{.friend_id = static_cast<uint64_t>(friend_id)};

    auto sender = connection_.sendRequest(request);

    return sender;
  }

  // auto sendMessage(const )

  auto Heartbeat() -> stdexec::sender auto {
    Request::Heartbeat request{};

    auto sender = connection_.sendRequest(request);

    return sender;
  }

  auto alive_listen() -> stdexec::sender auto {
    auto sender = connection_.listen() |
                  stdexec::let_error([this](std::exception_ptr ep) {
                    try {
                      std::rethrow_exception(ep);
                      return connection_.connect();
                    } catch (const std::exception &e) {
                      log(error, "Error in alive_listen: {}", e.what());
                      std::this_thread::sleep_for(std::chrono::seconds(5));
                      return connection_.connect();
                    }
                  }) |
                  exec::repeat_effect();
    return sender;
  }

  auto Echo(const std::string &message) -> stdexec::sender auto {
    Request::Echo request{.message = message};

    auto sender = connection_.sendRequest(request);

    return sender;
  }

  auto FindUsers(const std::string &query) -> stdexec::sender auto {
    Request::FindUsers request{.query = query};

    auto sender = connection_.sendRequest(request);

    return sender;
  }

  auto lowLevel() -> C & { return connection_; }

private:
  C connection_;
};
}; // namespace Logic
