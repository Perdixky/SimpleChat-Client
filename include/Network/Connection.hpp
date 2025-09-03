#pragma once
#include "Async/Loop.hpp"
#include "Network/Concepts.hpp"
#include "Network/MessageType.hpp"
#include "Network/ResponseRoute.hpp"
#include "Network/ResponseSender.hpp"

#include "boost/asio.hpp"
#include "rfl/Generic.hpp"
#include "rfl/Result.hpp"
#include <asioexec/use_sender.hpp>
#include <boost/asio/generic/detail/endpoint.hpp>
#include <boost/asio/ssl.hpp>
#include <expected>
#include <rfl/msgpack.hpp>
#include <stdexec/execution.hpp>

namespace Network {

template <ResponseRouterType R> class Connection {
public:
  Connection(const boost::asio::ip::tcp::endpoint endpoint, R &router_)
      : context_(boost::asio::ssl::context::tlsv13_client), endpoint_(endpoint),
        stream_(Async::Loop::getIOContext(), context_), router_(router_) {
    boost::system::error_code ec;

    context_.set_verify_mode(boost::asio::ssl::verify_peer, ec);
    if (ec) {
      throw std::runtime_error("Failed to set verify mode: " + ec.message());
    }

    const char certificate[] = {
#embed "../../safety/certificate.crt"
    };

    context_.add_certificate_authority(
        boost::asio::buffer(certificate, sizeof(certificate) - 1),
        ec); // -1 移除末尾的空字符
    if (ec) {
      // throw std::runtime_error("Failed to add certificate authority: " +
      //                          ec.message());
      // TODO: 添加错误处理
    }
  }

  // 如果在自己文件中定义会因为 auto 没有实例化导致 concept 断言失败
  auto connect() -> stdexec::sender auto {
    using namespace boost;

    stdexec::sender auto sender =
        stream_.next_layer().async_connect(endpoint_, asioexec::use_sender) |
        stdexec::let_value([this]() {
          return stream_.async_handshake(asio::ssl::stream_base::client,
                                         asioexec::use_sender);
        }) |
        stdexec::then([]() { return true; }) |
        stdexec::upon_error([](const std::exception_ptr e_ptr) {
          if (e_ptr) {
            try {
              std::rethrow_exception(e_ptr);
            } catch (const std::exception &e) {
              // TODO: Log the error message e.what()
              return false;
            }
          }
          return true; // 如果没有异常，返回 true
        });

    return sender;
  }

  template <Network::RequestType T>
  auto sendRequest(T &request) -> stdexec::sender auto {
    using namespace boost;

    // Added static to avoid re-initialization
    static boost::uuids::random_generator generator;

    request.id = boost::uuids::to_string(generator());

    auto request_msg = rfl::msgpack::write(request);

    auto uuid = generator();

    auto send =
        stdexec::when_all(stream_.async_write_some(asio::buffer(request_msg),
                                                   asioexec::use_sender),
                          stdexec::just([&uuid] { return uuid; })) |
        stdexec::let_value([this](auto, uuids::uuid uuid) {
          return ResponseSender<T, R>{router_, uuid};
        });

    return send;
  }

  // 需要在外部调用 repeat_effect 来持续监听
  auto listen() -> stdexec::sender auto {
    using namespace boost;
    using namespace stdexec;

    rfl::Error e("sdf");

    char buffer[4096];
    auto sender =
        stream_.async_read_some(asio::buffer(buffer), asioexec::use_sender) |
        then([&buffer, this](auto lenth) {
          auto generic = rfl::msgpack::read<rfl::Generic::Object>(
              std::span(buffer, lenth));
          if (generic) [[likely]] {
            auto value = generic.value();
            auto id = value.get("id");
            if (id) {
              router_.route(value);
            } else {
              // TODO: 处理没有 ID 的消息
            }
          } else {
            throw std::runtime_error(std::format("Failed to parse message: {}",
                                                 generic.error().what()));
          }
        });
    return sender;
  }

private:
  boost::asio::ssl::context context_;
  boost::asio::ip::tcp::endpoint endpoint_;
  boost::asio::ssl::stream<boost::asio::ip::tcp::socket> stream_;

  R &router_;
};

}; // namespace Network
