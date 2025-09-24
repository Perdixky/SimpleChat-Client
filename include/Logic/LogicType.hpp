#pragma once
#include <chrono>
#include <cstdint>
#include <optional>
#include <rfl.hpp>
#include <variant>

namespace Logic {

enum class FriendshipStatus : uint8_t {
  Pending = 0,
  Accepted = 1,
  Rejected = 2,
};

enum class ConversationType : uint8_t {
  Private = 0,
  Group = 1,
};

enum class ConversationRole : uint8_t {
  Member = 0,
  Admin = 1,
};

#undef Success
enum class LoginStatus : uint8_t {
  Success,
  UserNotFound,
  InvalidPassword,
};

enum class UserStatus : uint8_t {
  Offline = 0,
  Online = 1,
};

namespace Data {

struct Conversation {
  uint64_t id;
  std::string title;
  uint8_t type; // 0 = private, 1 = group
  rfl::Timestamp<"%Y-%m-%d %H:%M:%S"> updated_at;
};

struct Message {
  uint64_t id;
  uint64_t sender_id;
  std::string content;
  rfl::Timestamp<"%Y-%m-%d %H:%M:%S"> sent_at;
};

struct User {
  uint64_t id;
  std::string name;
  std::string email;
};

}; // namespace Data

namespace Message {

using SendMessage = Data::Message;

struct AddFriend {
  uint64_t id;
  uint64_t requester_id;
  uint64_t addressee_id;
  rfl::Timestamp<"%Y-%m-%d %H:%M:%S"> created_at;
  rfl::Timestamp<"%Y-%m-%d %H:%M:%S"> updated_at;
  uint8_t status; // 0 = pending, 1 = accepted, 2 = rejected
};

struct SignIn {
  uint64_t user_id;
  UserStatus status;
};

}; // namespace Message

namespace Response {
struct SignIn {
  std::string id;
  LoginStatus status;
  uint64_t user_id;
  // std::string message; // 例如错误信息
};

struct Heartbeat {
  std::string id;
  std::optional<std::string> error;
  uint64_t timestamp;
};

struct Echo {
  std::string id;
  std::string message;
};

struct SignUp {
  std::string id;
  bool success;
  // std::string message; // 例如错误信息
};

struct AddFriend {
  std::string id;
  bool success;
  std::string message;
};

struct FriendRequest {
  std::string id;
  bool success;
  std::string message;
};

struct CreateGroup {
  std::string id;
  bool success;
  uint64_t group_id;
  std::string message;
};

struct InviteToGroup {
  std::string id;
  bool success;
  std::string message;
};

struct SendMessage {
  std::string id;
  bool success;
  std::string message;
};

struct GetConversationList {
  std::string id;
  bool success;
  std::string message;
  std::vector<Logic::Data::Conversation> conversations;
};

struct GetMessageHistory {
  std::string id;
  std::vector<Logic::Data::Message> messages;
  bool success;
  std::string message;
};

struct FriendRequestResponse {
  std::string id;
  bool success;
  std::string message;
};

struct GetConversationMemberList {
  std::string id;
  std::vector<Data::User> members;
};

struct FindUsers {
  std::string id;
  std::vector<Data::User> users;
};

}; // namespace Response

namespace Request {

struct SignIn {
  using Response = Response::SignIn;
  using Message = Message::SignIn;
  static constexpr auto method_name = "SignIn";
  std::string id;

  std::string method = "SignIn";

  std::string email;
  std::string password;
};

struct SignUp {
  using Response = Response::SignUp;
  using Message = std::monostate;
  static constexpr auto method_name = "SignUp";
  std::string id;

  std::string username;
  std::string invite_code;
  std::string method = "SignUp";

  std::string email;
  std::string password;
};

struct Heartbeat {
  using Response = Response::Heartbeat;
  using Message = std::monostate;
  static constexpr auto method_name = "Heartbeat";
  std::string id;

  std::string method = "Heartbeat";
  uint64_t timestamp =
      std::chrono::steady_clock::now().time_since_epoch().count();
};

struct Echo {
  using Response = Response::Echo;
  using Message = std::monostate;
  static constexpr auto method_name = "Echo";
  std::string id;

  std::string method = "Echo";
  std::string message;
};

struct AddFriend {
  using Response = Response::AddFriend;
  using Message = Message::AddFriend;
  static constexpr auto method_name = "AddFriend";
  std::string id;
  std::string method = "AddFriend";
  uint64_t friend_id;
};

struct FriendRequestResponse {
  using Response = Response::FriendRequestResponse;
  using Message = std::monostate;
  static constexpr auto method_name = "FriendRequestResponse";
  std::string id;
  std::string method = "FriendRequestResponse";
  uint64_t request_id;
  bool accepted;
};

struct CreateGroup {
  using Response = Response::CreateGroup;
  using Message = std::monostate;
  static constexpr auto method_name = "CreateGroup";
  std::string id;
  std::string method = "CreateGroup";
  uint64_t user_id;
  std::string group_name;
  std::vector<uint64_t> member_ids;
};

struct InviteToGroup {
  using Response = Response::InviteToGroup;
  using Message = std::monostate;
  static constexpr auto method_name = "InviteToGroup";
  std::string id;
  std::string method = "InviteToGroup";
  uint64_t inviter_id;
  uint64_t group_id;
  std::vector<uint64_t> invitee_ids;
};

struct SendMessage {
  using Response = Response::SendMessage;
  using Message = Message::SendMessage;
  static constexpr auto method_name = "SendMessage";
  std::string id;
  std::string method = "SendMessage";
  uint64_t sender_id;
  uint64_t conversation_id;
  std::string content;
};

struct GetConversationList {
  using Response = Response::GetConversationList;
  using Message = std::monostate;
  static constexpr auto method_name = "GetConversationList";
  std::string id;
  std::string method = "GetConversationList";
};

struct GetMessageHistory {
  using Response = Response::GetMessageHistory;
  using Message = std::monostate;
  static constexpr auto method_name = "GetMessageHistory";
  std::string id;
  std::string method = "GetMessageHistory";
  uint64_t conversation_id;
  // uint32_t limit = 50; // Number of messages to retrieve
  // uint32_t offset = 0; // For pagination
};

struct GetConversationMemberList {
  using Response = Response::GetConversationMemberList;
  using Message = std::monostate;
  static constexpr auto method_name = "GetConversationMemberList";
  std::string id;
  std::string method = "GetConversationMemberList";
  uint64_t conversation_id;
};

struct FindUsers {
  using Response = Response::FindUsers;
  using Message = std::monostate;
  static constexpr auto method_name = "FindUsers";
  std::string id;
  std::string method = "FindUsers";
  std::string query;
};

}; // namespace Request

}; // namespace Logic
