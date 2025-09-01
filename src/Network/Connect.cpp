#include "Network/Connection.hpp"
#include <boost/system/detail/error_code.hpp>

using namespace boost;
auto Network::Connection::connect()
    -> stdexec::sender auto {
  stdexec::sender auto sender =
      stream_.next_layer().async_connect(endpoint, asioexec::use_sender) |
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
  // return std::get<0>(*stdexec::sync_wait(std::move(sender)));
}
