#pragma once
#include <boost/asio.hpp>
#include <boost/system/detail/error_code.hpp>
#include <concepts>
#include <exec/async_scope.hpp>
#include <stdexec/execution.hpp>
#include <execpools/asio/asio_thread_pool.hpp>
#include <Utils/Logger.hpp>

namespace Async {
class Loop {
public:
  template <std::invocable Func>
  static auto call_soon(const Func &&func) -> void {
    boost::asio::post(io_context_, std::forward<Func>(func));
  }

  template <stdexec::sender S> static auto submit(S &&sender) -> void {
    scope_.spawn(std::forward<S>(sender));
  }

  template <std::invocable Func, typename Duration>
  static auto startTimer(Func &&func, const Duration &interval) -> void {
    std::function<void(const boost::system::error_code &)> handle_timeout;

    handle_timeout = [func = std::forward<Func>(func), interval,
                      &handle_timeout](const boost::system::error_code &ec) {
      if (ec) {
        // TODO: 添加错误处理
        // std::print("Timer cancelled or error: {}\n", ec.message());
        return;
      }

      func();

      timer_.expires_at(timer_.expiry() + interval);

      // 这里要想递归handle_timeout必须将定义与赋值分开
      timer_.async_wait(handle_timeout);
    };

    // 首次启动
    timer_.expires_after(interval);
    timer_.async_wait(handle_timeout);
  }

  static auto cancelTimer() -> void { timer_.cancel(); }

  static auto run() -> void { 
    LOG(info) << "Async Loop started.";
    io_context_.get_executor().context().join();
    LOG(info) << "Async Loop stopped.";
  }

  static auto stop() -> void { 
    scope_.request_stop();
    io_context_.get_executor().context().stop();
  }

  static auto getIOContext() -> auto & {
    return io_context_;
  }

private:
  static execpools::asio_thread_pool io_context_;
  static boost::asio::steady_timer timer_;
  static exec::async_scope scope_;
  static boost::asio::executor_work_guard<decltype(io_context_.get_executor())> work_guard_;
};

}; // namespace Async
