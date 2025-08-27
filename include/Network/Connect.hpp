#include "Async/EventLoop.hpp"
#include <asioexec/use_sender.hpp>
#include <boost/asio/ssl.hpp>
#include <stdexec/execution.hpp>

auto connect(const boost::asio::ip::tcp::endpoint &endpoint) -> stdexec::sender
    auto;
