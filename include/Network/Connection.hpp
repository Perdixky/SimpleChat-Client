#pragma once
#include "Async/Loop.hpp"
#include "Network/Concepts.hpp"
#include "Network/MessageType.hpp"
#include "Network/ResponseRoute.hpp"
#include "Network/ResponseSender.hpp"
#include "Utils/Logger.hpp"

#include "rfl/Generic.hpp"
#include "rfl/Result.hpp"
#include <asioexec/use_sender.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <exec/repeat_effect_until.hpp>
#include <expected>
#include <rfl/msgpack.hpp>
#include <stdexec/execution.hpp>

namespace Network {

template <ResponseRouterType R> class Connection {
public:
  Connection(const boost::asio::ip::tcp::endpoint endpoint, R &router_)
      : context_(boost::asio::ssl::context::tlsv13_client), endpoint_(endpoint),
        stream_(Async::Loop::getIOContext().get_executor(), context_),
        router_(router_) {
    LOG(info) << "Initializing connection to endpoint: " << endpoint_;
    boost::system::error_code ec;

    context_.set_verify_mode(boost::asio::ssl::verify_peer, ec);
    if (ec) {
      LOG(fatal) << "Failed to set verify mode: " << ec.message();
      throw std::runtime_error("Failed to set verify mode: " + ec.message());
    }

    const char certificate[] = {
#embed "../../safety/certificate.crt"
    };

    context_.add_certificate_authority(
        boost::asio::buffer(certificate, sizeof(certificate) - 1),
        ec); // -1 移除末尾的空字符
    if (ec) {
      LOG(error) << "Failed to add certificate authority: " << ec.message();
    }

    stream_.binary(true);
    LOG(info) << "Connection initialized successfully.";
  }

  auto connect() -> stdexec::sender auto {
    using namespace boost;
    LOG(info) << "Attempting to connect to " << endpoint_;

    stdexec::sender auto sender =
        stream_.next_layer().next_layer().async_connect(endpoint_,
                                                        asioexec::use_sender) |
        stdexec::let_value([this]() {
          LOG(info) << "TCP connection established. Starting SSL handshake...";
          return stream_.next_layer().async_handshake(
              asio::ssl::stream_base::client, asioexec::use_sender);
        }) |
        stdexec::let_value([this]() {
          LOG(info)
              << "SSL handshake successful. Starting WebSocket handshake...";
          stream_.set_option(
              boost::beast::websocket::stream_base::decorator([](auto &req) {
                req.set(boost::beast::http::field::user_agent,
                        std::string(BOOST_BEAST_VERSION_STRING) +
                            " SimpleChat-Client");
              }));
          return stream_.async_handshake("localhost", "/",
                                         asioexec::use_sender);
        }) |
        stdexec::then([this]() {
          LOG(info)
              << "WebSocket handshake successful. Connection established.";
        }) |
        stdexec::upon_error([this](const std::exception_ptr ec) {
          if (ec) {
            try {
              std::rethrow_exception(ec);
            } catch (const std::exception &e) {
              LOG(error) << "Connection failed: " << e.what();
            }
          }
        });

    return sender;
  }

  template <Network::RequestType T>
  auto sendRequest(T &request) -> stdexec::sender auto {
    using namespace boost;

    static boost::uuids::random_generator generator;
    auto uuid_obj = generator();
    request.id = boost::uuids::to_string(uuid_obj);

    LOG(debug) << "Sending request with ID: " << request.id;

    static std::vector<char> request_msg;
    request_msg = rfl::msgpack::write(request);

    auto send =
        stdexec::when_all(stream_.async_write(asio::buffer(request_msg),
                                              asioexec::use_sender),
                          stdexec::just(uuid_obj)) |
        stdexec::let_value([this](auto, uuids::uuid uuid) {
          LOG(trace) << "Successfully wrote message with ID: "
                     << boost::uuids::to_string(uuid) << " to socket.";
          return ResponseSender<T, R>{router_, uuid};
        });
      // |
      //   stdexec::upon_error([id = request.id](const std::exception_ptr ec) {
      //     if (ec) {
      //       try {
      //         std::rethrow_exception(ec);
      //       } catch (const std::exception &e) {
      //         LOG(error) << "Failed to send request with ID: " << id
      //                    << ", error: " << e.what();
      //         return T{};
      //       }
      //     }
      //   });

    return send;
  }

  auto listen() -> stdexec::sender auto {
    using namespace boost;
    using namespace stdexec;

    static auto buffer = beast::flat_buffer();
    LOG(trace) << "Listening for incoming messages...";
    auto sender =
        stream_.async_read(buffer, asioexec::use_sender) |
        then([this](auto length) {
          LOG(trace) << "Received " << length << " bytes.";
          auto generic = rfl::msgpack::read<rfl::Generic::Object>(std::span(
              static_cast<const char *>(buffer.data().data()), buffer.size()));
          if (generic) [[likely]] {
            auto value = generic.value();
            auto id = value.get("id");
            if (id) {
              router_.route(value);
            } else {
              LOG(warning) << "Received a message without an ID.";
            }
          } else {
            LOG(error) << "Failed to parse message: " << generic.error().what();
            throw std::runtime_error(
                std::format("Failed to parse message: {}, because {} ",
                            std::string_view(
                                static_cast<const char *>(buffer.data().data()),
                                buffer.size()),
                            generic.error().what()));
          }
          buffer.consume(buffer.size());
        });
    // |
    //     stdexec::upon_error([](const boost::system::error_code&ec) {
    //       LOG(error) << "Error in listen loop: " << ec.message();
    //     });
    return sender;
  }

private:
  boost::asio::ssl::context context_;
  boost::asio::ip::tcp::endpoint endpoint_;
  boost::beast::websocket::stream<
      boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>
      stream_;

  R &router_;
};

}; // namespace Network
