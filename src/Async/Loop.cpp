#include "Async/Loop.hpp"

boost::asio::io_context Async::Loop::io_context_;
boost::asio::steady_timer Async::Loop::timer_(io_context_);
