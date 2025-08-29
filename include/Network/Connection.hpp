#pragma once
#include "Async/Loop.hpp"
#include "boost/asio.hpp"
#include <asioexec/use_sender.hpp>
#include <boost/asio/generic/detail/endpoint.hpp>
#include <boost/asio/ssl.hpp>
#include <stdexec/execution.hpp>
#include "Network/MessageType.hpp"
#include "Network/ResponseRoute.hpp"


namespace Network {

class Connection : std::enable_shared_from_this<Connection> {
public:
  Connection(const boost::asio::ip::tcp::endpoint endpoint)
      : context_(boost::asio::ssl::context::tlsv13_client), endpoint_(endpoint),
        stream_(Async::Loop::getIOContext(), context_) {
    boost::system::error_code ec;

    context_.set_verify_mode(boost::asio::ssl::verify_peer, ec);
    if (ec) {
      // 强烈建议处理错误，而不是忽略它
      throw std::runtime_error("Failed to set verify mode: " + ec.message());
    }

    const char certificate[] = {
#embed "../../safety/certificate.crt"
    };

    context_.add_certificate_authority(
        boost::asio::buffer(certificate, sizeof(certificate) - 1),
        ec); // -1 移除末尾的空字符
    if (ec) {
      throw std::runtime_error("Failed to add certificate authority: " +
                               ec.message());
    }
  }

  // auto stream() -> boost::asio::ssl::stream<boost::asio::ip::tcp::socket> & {
  //   return stream_;
  // }

  auto connect() -> bool;

  template <Network::RequestType T>
  auto sendRequest(const T &request) -> T::ResponseType;

  auto listen() -> void;

private:
  boost::asio::ssl::context context_;
  boost::asio::ip::tcp::endpoint endpoint_;
  boost::asio::ssl::stream<boost::asio::ip::tcp::socket> stream_;

  ResponseRouter router_;
};

}; // namespace Network
