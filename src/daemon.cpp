// Electron bridge daemon: stdio JSON-RPC <-> existing C++ Session API
// - Reads one JSON object per line from stdin: {"rpc_id":N,"method":"Name","params":[...]} or named args
// - Writes one JSON object per line to stdout: {"rpc_id":N,"ok":true,"result":<Response>} or {..,"ok":false,"error":"..."}

#include "Async/Loop.hpp"
#include "Logic/Session.hpp"
#include "Network/Connection.hpp"
#include "Utils/Logger.hpp"

#include <asioexec/use_sender.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <rfl/Generic.hpp>
#include <rfl/json.hpp>
#include <stdexec/execution.hpp>

#include <atomic>
#include <chrono>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <optional>
#include <print>
#include <sstream>
#include <string>
#include <thread>

namespace {

// Minimal JSON-string escaper for error messages
static std::string json_escape(std::string s) {
  std::string out;
  out.reserve(s.size() + 8);
  for (char c : s) {
    switch (c) {
    case '"': out += "\\\""; break;
    case '\\': out += "\\\\"; break;
    case '\n': out += "\\n"; break;
    case '\r': out += "\\r"; break;
    case '\t': out += "\\t"; break;
    default:
      if (static_cast<unsigned char>(c) < 0x20) {
        char buf[7]{};
        std::snprintf(buf, sizeof(buf), "\\u%04x", (unsigned)c);
        out += buf;
      } else {
        out += c;
      }
    }
  }
  return out;
}

// Read helper for either params-array or named field
template <typename T>
std::optional<T> get_arg(const rfl::Generic::Object &obj, std::size_t index,
                         const char *name) {
  if (auto params = obj.get("params")) {
    if (auto arr_opt = params->to_array()) {
      const auto &arr = arr_opt.value();
      if (index < arr.size()) {
        auto v = rfl::from_generic<T>(arr[index]);
        if (v)
          return v.value();
      }
    }
  }
  if (auto v = obj.get(name)) {
    auto res = rfl::from_generic<T>(*v);
    if (res)
      return res.value();
  }
  return std::nullopt;
}

// Write a success envelope to stdout; result_json must be a valid JSON object/array/scalar string
static void write_ok(uint64_t rpc_id, const std::string &result_json) {
  std::cout << "{\"rpc_id\":" << rpc_id << ",\"ok\":true,\"result\":"
            << result_json << "}\n";
  std::cout.flush();
}

static void write_err(uint64_t rpc_id, const std::string &message) {
  std::cout << "{\"rpc_id\":" << rpc_id << ",\"ok\":false,\"error\":\""
            << json_escape(message) << "\"}\n";
  std::cout.flush();
}

} // namespace

int main(int argc, char **argv) {
  // Initialize logging to stderr to avoid mixing with JSON stdout
  initLogging();
  try {
    auto err_logger = spdlog::stderr_color_mt("electron-daemon");
    spdlog::set_default_logger(err_logger);
  } catch (...) {
    // ignore if already exists
  }
  spdlog::set_level(spdlog::level::info);
  spdlog::set_pattern("[%H:%M:%S.%f] [%^%l%$] [%t] %v");

  log(info, "Daemon starting (Electron bridge)");

  // Endpoint configuration: reuse current defaults from src/main.cpp
  auto endpoint = boost::asio::ip::tcp::endpoint(
      boost::asio::ip::make_address("127.0.0.1"), 8888);

  Network::ResponseRouter router;
  Network::Connection connection(std::move(endpoint), router);
  Logic::Session<decltype(connection)> session(std::move(connection));

  // Connect once and keep listen-alive
  try {
    stdexec::sync_wait(session.lowLevel().connect());
  } catch (const std::exception &e) {
    log(error, "Initial connect failed: {}", e.what());
  }
  Async::Loop::submit(session.alive_listen());

  std::jthread async_thread([] { Async::Loop::run(); });

  std::atomic<bool> stopping{false};

  // Input loop on main thread: line-delimited JSON
  std::string line;
  while (std::getline(std::cin, line)) {
    if (line.empty())
      continue;
    auto parsed = rfl::json::read<rfl::Generic::Object>(line);
    if (!parsed) {
      // Can't parse; no rpc_id to reply to
      log(warning, "Malformed JSON from stdin: {}", line);
      continue;
    }
    const auto &obj = parsed.value();

    // rpc_id default 0 if missing
    uint64_t rpc_id = 0;
    if (auto idv = obj.get("rpc_id")) {
      if (auto id64 = rfl::from_generic<uint64_t>(*idv)) {
        rpc_id = *id64;
      }
    }

    auto method = obj.get("method").and_then(rfl::to_string);
    if (!method) {
      write_err(rpc_id, "Missing method");
      continue;
    }

    const auto &m = *method;
    log(debug, "RPC {} called", m);

    auto submit_sender = [&](auto &&sender) {
      using Sender = decltype(sender);
      Async::Loop::submit(std::forward<Sender>(sender) |
                          stdexec::then([rpc_id](auto &&result) {
                            // Serialize the plain response into JSON and wrap
                            auto json = rfl::json::write(result);
                            write_ok(rpc_id, json);
                          }) |
                          stdexec::upon_error([rpc_id](std::exception_ptr ep) {
                            std::string msg = "unknown error";
                            if (ep) {
                              try {
                                std::rethrow_exception(ep);
                              } catch (const std::exception &e) {
                                msg = e.what();
                              } catch (...) {
                              }
                            }
                            write_err(rpc_id, msg);
                          }));
    };

    try {
      if (m == "SignIn") {
        auto email = get_arg<std::string>(obj, 0, "email");
        auto password = get_arg<std::string>(obj, 1, "password");
        if (!email || !password) {
          write_err(rpc_id, "SignIn requires email and password");
          continue;
        }
        submit_sender(session.SignIn(*email, *password));
      } else if (m == "SignUp") {
        auto username = get_arg<std::string>(obj, 0, "username");
        auto invite_code = get_arg<std::string>(obj, 1, "invite_code");
        auto email = get_arg<std::string>(obj, 2, "email");
        auto verification_code = get_arg<std::string>(obj, 3, "verification_code");
        auto password = get_arg<std::string>(obj, 4, "password");
        if (!username || !invite_code || !email || !password) {
          write_err(rpc_id, "SignUp requires username, invite_code, email, password");
          continue;
        }
        submit_sender(session.SignUp(*username, *invite_code, *email,
                                     verification_code.value_or(""), *password));
      } else if (m == "GetConversationList") {
        submit_sender(session.GetConversationList());
      } else if (m == "GetConversationMemberList") {
        auto conv_id = get_arg<int>(obj, 0, "conversation_id");
        if (!conv_id) {
          write_err(rpc_id, "GetConversationMemberList requires conversation_id");
          continue;
        }
        submit_sender(session.GetConversationMemberList(*conv_id));
      } else if (m == "GetMessageHistory") {
        auto conv_id = get_arg<int>(obj, 0, "conversation_id");
        if (!conv_id) {
          write_err(rpc_id, "GetMessageHistory requires conversation_id");
          continue;
        }
        submit_sender(session.GetMessageHistory(*conv_id));
      } else if (m == "AddFriend") {
        auto fid = get_arg<int>(obj, 0, "friend_id");
        if (!fid) {
          write_err(rpc_id, "AddFriend requires friend_id");
          continue;
        }
        submit_sender(session.AddFriend(*fid));
      } else if (m == "FindUsers") {
        auto q = get_arg<std::string>(obj, 0, "query");
        if (!q) {
          write_err(rpc_id, "FindUsers requires query");
          continue;
        }
        submit_sender(session.FindUsers(*q));
      } else if (m == "Echo") {
        auto msg = get_arg<std::string>(obj, 0, "message");
        submit_sender(session.Echo(msg.value_or("")));
      } else if (m == "Heartbeat") {
        submit_sender(session.Heartbeat());
      } else if (m == "Quit" || m == "Exit") {
        write_ok(rpc_id, "true");
        break;
      } else {
        write_err(rpc_id, std::string("Unknown method: ") + m);
      }
    } catch (const std::exception &e) {
      write_err(rpc_id, e.what());
    }
  }

  stopping = true;
  Async::Loop::stop();
  log(info, "Daemon exiting");
  return 0;
}
