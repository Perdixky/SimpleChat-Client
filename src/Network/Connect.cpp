#include "Network/Connect.hpp"
#include <boost/system/detail/error_code.hpp>

using namespace boost;
auto connect(const asio::ip::tcp::endpoint &endpoint) -> stdexec::sender auto {
  asio::ssl::context ssl_ctx(asio::ssl::context::tlsv13_client);
  boost::system::error_code ec;
  ec = ssl_ctx.set_verify_mode(asio::ssl::verify_peer, ec);
  if (ec) {
    // throw std::runtime_error("Failed to set verify mode: " + ec.message());
  }
  char certificate[] = {
#embed "../../safety/certificate.crt"
  };
  ec = ssl_ctx.add_certificate_authority(
      asio::buffer(certificate, sizeof(certificate)), ec);

  asio::ssl::stream<asio::ip::tcp::socket> stream(Async::loop.getIOContext(), ssl_ctx);

  stdexec::sender auto sender =
      stream.next_layer().async_connect(endpoint, asioexec::use_sender) |
      stdexec::let_value([&stream]() {
        return stream.async_handshake(asio::ssl::stream_base::client,
                                      asioexec::use_sender);
      }) |
      stdexec::upon_error([](const std::exception_ptr e_ptr) {
        if (e_ptr) {
          try {
            std::rethrow_exception(e_ptr);
          } catch (const std::exception &e) {
          }
        }
      });
  return sender;
}
