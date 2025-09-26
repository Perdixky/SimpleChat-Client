#include "Async/Loop.hpp"
#include "Logic/Session.hpp"
#include "Network/Connection.hpp"
#include "Network/ResponseRoute.hpp"
#include "Utils/FunctionTraits.hpp"
#include "Utils/Logger.hpp"
#include <boost/asio/ip/tcp.hpp>
#include <rfl/MetaField.hpp>
#include <rfl/json.hpp>

#include <napi.h>

namespace {

// Type aliases for cleaner code
using ConnectionType = Network::Connection<Network::ResponseRouter>;
using SessionType = Logic::Session<ConnectionType>;

// Helper to extract error message from exception pointer
std::string extractErrorMessage(std::exception_ptr ep) {
  if (!ep)
    return "unknown error";

  try {
    std::rethrow_exception(ep);
  } catch (const std::exception &e) {
    return e.what();
  } catch (...) {
    return "unknown error";
  }
}

// Parameter deserializer template
template <typename T> T deserializeArg(const Napi::Value &value) {
  if constexpr (std::is_same_v<T, std::string>) {
    return value.As<Napi::String>().Utf8Value();
  } else if constexpr (std::is_same_v<T, int>) {
    return value.As<Napi::Number>().Int32Value();
  } else if constexpr (std::is_same_v<T, uint64_t>) {
    return static_cast<uint64_t>(value.As<Napi::Number>().Int64Value());
  } else if constexpr (std::is_same_v<T, bool>) {
    return value.As<Napi::Boolean>().Value();
  }
  // Add more types as needed
  static_assert(std::is_same_v<T, std::string> || std::is_same_v<T, int> ||
                    std::is_same_v<T, uint64_t> || std::is_same_v<T, bool>,
                "Unsupported parameter type");
}

// Deserialize arguments to tuple
template <typename... Args, size_t... Is>
auto deserializeArgs(const Napi::CallbackInfo &info,
                     std::index_sequence<Is...>) {
  return std::make_tuple(deserializeArg<Args>(info[Is])...);
}

template <typename T>
concept StdVector =
    requires {
      // 必须有这些成员类型
      typename T::value_type;
      typename T::allocator_type;
      typename T::size_type;
      typename T::iterator;
    } &&
    requires(T t, typename T::size_type i) {
      // 必须有这些操作
      { t.size() } -> std::convertible_to<typename T::size_type>;
      { t[i] } -> std::convertible_to<typename T::value_type &>;
      { t.begin() } -> std::same_as<typename T::iterator>;
      { t.end() } -> std::same_as<typename T::iterator>;
    } &&
    std::same_as<
        T, std::vector<typename T::value_type, typename T::allocator_type>>;

// -------- JSON/JS Serialization Helpers --------

// Compile-time false dependent helper for static_assert fallbacks
template <class T> struct always_false : std::false_type {};
template <class T> inline constexpr bool always_false_v = always_false<T>::value;

// Basic type traits
template <typename T> struct is_std_vector : std::false_type {};
template <typename T, typename A>
struct is_std_vector<std::vector<T, A>> : std::true_type {};

template <typename T> struct is_std_optional : std::false_type {};
template <typename T>
struct is_std_optional<std::optional<T>> : std::true_type {};

// rfl::Timestamp detection
template <typename T> struct is_rfl_timestamp : std::false_type {};
template <auto Format>
struct is_rfl_timestamp<rfl::Timestamp<Format>> : std::true_type {};

// Forward decl
template <typename T> Napi::Object serializeObject(const Napi::Env &env, const T &v);

// Convert any supported C++ value to a Napi::Value
template <typename T>
Napi::Value toJs(const Napi::Env &env, const T &value) {
  using V = std::remove_cvref_t<T>;

  if constexpr (std::is_same_v<V, std::string>) {
    return Napi::String::New(env, value);
  } else if constexpr (std::is_same_v<V, const char *>) {
    return Napi::String::New(env, value);
  } else if constexpr (std::is_same_v<V, bool>) {
    return Napi::Boolean::New(env, value);
  } else if constexpr (std::is_enum_v<V>) {
    using U = std::underlying_type_t<V>;
    if constexpr (sizeof(U) >= 8) {
      // 64-bit enum underlying type: use BigInt to preserve full precision
      if constexpr (std::is_signed_v<U>) {
        return Napi::BigInt::New(env, static_cast<int64_t>(static_cast<U>(value)));
      } else {
        return Napi::BigInt::New(env, static_cast<uint64_t>(static_cast<U>(value)));
      }
    } else {
      return Napi::Number::New(env, static_cast<double>(static_cast<U>(value)));
    }
  } else if constexpr (std::is_integral_v<V> && sizeof(V) < 8) {
    return Napi::Number::New(env, static_cast<double>(value));
  } else if constexpr (std::is_integral_v<V> && sizeof(V) == 8) {
    if constexpr (std::is_signed_v<V>) {
      return Napi::BigInt::New(env, static_cast<int64_t>(value));
    } else {
      return Napi::BigInt::New(env, static_cast<uint64_t>(value));
    }
  } else if constexpr (std::is_floating_point_v<V>) {
    return Napi::Number::New(env, static_cast<double>(value));
  } else if constexpr (is_rfl_timestamp<V>::value) {
    // Prefer human-readable string for timestamp
    // rfl::to_string(Timestamp) typically returns std::string or expected<std::string>
    if constexpr (requires { rfl::to_string(value); }) {
      auto s = rfl::to_string(value);
      if constexpr (std::is_convertible_v<decltype(s), std::string>) {
        return Napi::String::New(env, s);
      } else {
        return Napi::String::New(env, s.value());
      }
    } else {
      // Fallback: serialize via JSON writer (results in quoted string)
      return Napi::String::New(env, rfl::json::write(value));
    }
  } else if constexpr (is_std_optional<V>::value) {
    if (!value.has_value()) {
      return env.Null();
    }
    return toJs(env, *value);
  } else if constexpr (is_std_vector<V>::value) {
    Napi::Array arr = Napi::Array::New(env, value.size());
    for (size_t i = 0; i < value.size(); ++i) {
      arr.Set(i, toJs(env, value[i]));
    }
    return arr;
  } else if constexpr (requires { rfl::to_view(value); }) {
    // Treat reflectable objects as JS objects
    return serializeObject(env, value);
  } else {
    static_assert(always_false_v<V>, "Unsupported field type in toJs");
  }
}

// Serialize reflectable object using rfl view
template <typename T>
Napi::Object serializeResult(const Napi::Env &env, const T &v) {
  auto obj = Napi::Object::New(env);
  const auto view = rfl::to_view(v);
  view.apply([&](const auto &f) {
    using FieldT = std::remove_cvref_t<decltype(*f.value())>;
    // Dereference pointer to field value
    const auto &field_value = *f.value();
    obj.Set(std::string{f.name()}, toJs(env, field_value));
  });
  return obj;
}

// Auto method wrapper using FunctionTraits for parameter deduction
template <typename SessionType, typename Method> class AutoMethodWrapper {
public:
  using Traits = Utils::function_traits<Method>;
  using ArgsType = typename Traits::decayed_args_as_tuple;
  static constexpr size_t ArgCount = Traits::arity;

  static Napi::Value wrap(SessionType *session, Method method,
                          const Napi::CallbackInfo &info) {
    auto env = info.Env();
    auto deferred = Napi::Promise::Deferred::New(env);

    try {
      // Direct deserialization and method call without intermediate tuple
      auto sender = deserializeAndCall<ArgCount>(
          session, method, info, std::make_index_sequence<ArgCount>{});

      // Create thread-safe callback
      auto tsfn = Napi::ThreadSafeFunction::New(
          env, Napi::Function::New(env, [](const Napi::CallbackInfo &) {}),
          "method_callback", 0, 1);

      // Submit to async loop
      Async::Loop::submit(
          std::move(sender) |
          stdexec::then([deferred, tsfn](auto result) mutable {
            auto callback = [deferred = std::move(deferred),
                             result = std::move(result)](
                                Napi::Env env, Napi::Function) mutable {
              try {
                auto js_result = serializeResult(env, result);
                deferred.Resolve(js_result);
              } catch (const std::exception &e) {
                deferred.Reject(Napi::Error::New(env, e.what()).Value());
              }
            };
            tsfn.BlockingCall(callback);
            tsfn.Release();
          }) |
          stdexec::upon_error([deferred, tsfn](std::exception_ptr ep) mutable {
            auto callback = [deferred = std::move(deferred),
                             ep](Napi::Env env, Napi::Function) mutable {
              std::string msg = extractErrorMessage(ep);
              deferred.Reject(Napi::Error::New(env, msg).Value());
            };
            tsfn.BlockingCall(callback);
            tsfn.Release();
          }));

    } catch (const std::exception &e) {
      deferred.Reject(Napi::Error::New(env, e.what()).Value());
    }

    return deferred.Promise();
  }

private:
  template <size_t ArgCount, size_t... Is>
  static auto deserializeAndCall(SessionType *session, Method method,
                                 const Napi::CallbackInfo &info,
                                 std::index_sequence<Is...>) {
    // Direct parameter expansion without intermediate tuple
    return (session->*method)(
        deserializeArg<std::tuple_element_t<Is, ArgsType>>(info[Is])...);
  }
};

// Macro for easy method wrapping (requires semicolon)
#define WRAP_METHOD(method_name)                                               \
  Napi::Value method_name(const Napi::CallbackInfo &info) {                    \
    return AutoMethodWrapper<                                                  \
        SessionType,                                                           \
        decltype(&SessionType::method_name)>::wrap(session_.get(),             \
                                                   &SessionType::method_name,  \
                                                   info);                      \
  }                                                                            \
  static_assert(true, "") // Force semicolon requirement

} // anonymous namespace

// SessionWrapper class
class SessionWrapper : public Napi::ObjectWrap<SessionWrapper> {
private:
  std::unique_ptr<Network::ResponseRouter> router_;
  std::unique_ptr<SessionType> session_;
  std::jthread loop_thread_;
  Napi::ThreadSafeFunction event_tsfn_{};

public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(
        env, "Session",
        {// Expose PascalCase to match Logic::Request::<Name>::method_name
         InstanceMethod("SignIn", &SessionWrapper::SignIn),
         InstanceMethod("SignUp", &SessionWrapper::SignUp),
         InstanceMethod("GetConversationList",
                        &SessionWrapper::GetConversationList),
         InstanceMethod("GetConversationMemberList",
                        &SessionWrapper::GetConversationMemberList),
         InstanceMethod("GetMessageHistory",
                        &SessionWrapper::GetMessageHistory),
         InstanceMethod("AddFriend", &SessionWrapper::AddFriend),
         InstanceMethod("FindUsers", &SessionWrapper::FindUsers),
         InstanceMethod("Heartbeat", &SessionWrapper::Heartbeat),
         InstanceMethod("Echo", &SessionWrapper::Echo),
         // Event subscription from JS (main process)
         InstanceMethod("OnEvent", &SessionWrapper::OnEvent)});

    exports.Set("Session", func);
    return exports;
  }

  SessionWrapper(const Napi::CallbackInfo &info)
      : Napi::ObjectWrap<SessionWrapper>(info) {

    // Initialize logging to stderr
    initLogging();
    try {
      auto logger = spdlog::stderr_color_mt("session-addon");
      spdlog::set_default_logger(logger);
    } catch (...) {
      // ignore if already exists
    }

    log(::info, "Initializing SessionWrapper");

    // Create connection - reuse daemon.cpp logic
    auto endpoint = boost::asio::ip::tcp::endpoint(
        boost::asio::ip::make_address("127.0.0.1"), 8888);

    router_ = std::make_unique<Network::ResponseRouter>();
    auto connection = ConnectionType(std::move(endpoint), *router_);
    session_ = std::make_unique<SessionType>(std::move(connection));

    // Wire router broadcast to JS event callback if present
    router_->setEventCallback([this](const rfl::Generic::Object &obj) {
      if (!event_tsfn_)
        return;
      // Create a small snapshot (type + raw) to JS; avoid heavy conversion for
      // now
      auto call = [obj](Napi::Env env, Napi::Function jsCb) {
        Napi::Object evt = Napi::Object::New(env);
        // try to extract 'type' if present
        try {
          if (auto t = obj.get("type").and_then(rfl::to_string); t) {
            evt.Set("type", Napi::String::New(env, t.value()));
          } else {
            evt.Set("type", Napi::String::New(env, "broadcast"));
          }
          if (auto id = obj.get("id").and_then(rfl::to_string); id) {
            evt.Set("id", Napi::String::New(env, id.value()));
          }
        } catch (...) {
        }
        // best-effort textual dump
        try {
          evt.Set("raw", Napi::String::New(env, rfl::json::write(obj)));
        } catch (...) {
        }
        jsCb.Call({evt});
      };
      event_tsfn_.BlockingCall(call);
    });

    // Start async loop thread
    loop_thread_ = std::jthread([] {
      log(::info, "Starting async loop thread");
      Async::Loop::run();
    });

    // Connect and start listening
    try {
      log(::info, "Attempting initial connection");
      stdexec::sync_wait(session_->lowLevel().connect());
      log(::info, "Connection established, starting alive_listen");
      Async::Loop::submit(session_->alive_listen());
    } catch (const std::exception &e) {
      log(error, "Failed to initialize connection: {}", e.what());
    }
  }

  ~SessionWrapper() {
    log(::info, "Destroying SessionWrapper");
    Async::Loop::stop();
    if (loop_thread_.joinable()) {
      loop_thread_.join();
    }
    if (event_tsfn_) {
      event_tsfn_.Release();
    }
  }

private:
  // Instance methods exposed to JS (PascalCase) - using auto wrapper macro
  WRAP_METHOD(SignIn);
  WRAP_METHOD(SignUp);
  WRAP_METHOD(GetConversationList);
  WRAP_METHOD(GetConversationMemberList);
  WRAP_METHOD(GetMessageHistory);
  WRAP_METHOD(AddFriend);
  WRAP_METHOD(FindUsers);
  WRAP_METHOD(Heartbeat);
  WRAP_METHOD(Echo);

  // Allow JS to register an event callback; events originate from
  // server push packets (no 'id') routed via ResponseRouter::broadcast
  Napi::Value OnEvent(const Napi::CallbackInfo &info) {
    auto env = info.Env();
    if (info.Length() < 1 || !info[0].IsFunction()) {
      Napi::TypeError::New(env, "OnEvent expects a function")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (event_tsfn_) {
      event_tsfn_.Release();
    }
    auto cb = info[0].As<Napi::Function>();
    event_tsfn_ = Napi::ThreadSafeFunction::New(env, cb, "session_event", 0, 1);
    return env.Undefined();
  }
};

// Module initialization
Napi::Object Init(Napi::Env env, Napi::Object exports) {
  return SessionWrapper::Init(env, exports);
}

NODE_API_MODULE(session_addon, Init)
