#include "Async/Loop.hpp"
#include <boost/asio/executor_work_guard.hpp>

execpools::asio_thread_pool Async::Loop::io_context_(1);
boost::asio::steady_timer Async::Loop::timer_(io_context_.get_executor());
exec::async_scope Async::Loop::scope_;
boost::asio::executor_work_guard<decltype(Async::Loop::io_context_.get_executor())> Async::Loop::work_guard_ =
    boost::asio::make_work_guard(Async::Loop::io_context_.get_executor());
