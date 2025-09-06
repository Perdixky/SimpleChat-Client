#include "Async/Loop.hpp"

execpools::asio_thread_pool Async::Loop::io_context_(1);
boost::asio::steady_timer Async::Loop::timer_(io_context_.get_executor());
exec::async_scope Async::Loop::scope_;
