#pragma once
#include <boost/asio.hpp>
#include <boost/system/detail/error_code.hpp>
#include <concepts>
#include <print>
#include <stdexec/execution.hpp>

namespace Async {
class Loop {
public:

  template <std::invocable Func>
  static auto call_soon(const Func &&func) -> void {
    boost::asio::post(io_context_, std::forward<Func>(func));
  }

  template <stdexec::sender S> auto submit(S &&sender) -> void {
    stdexec::start_detached(std::forward<S>(sender));
  }

  template <std::invocable Func, typename Duration>
  static auto startTimer(Func &&func, const Duration &interval) -> void {
    std::function<void(const boost::system::error_code &)> handle_timeout;

    handle_timeout = [func = std::forward<Func>(func), interval,
                      &handle_timeout](const boost::system::error_code &ec) {
      if (ec) {
        std::print("Timer cancelled or error: {}\n", ec.message());
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

  static auto run() -> void { io_context_.run(); }

  static auto getIOContext() -> boost::asio::io_context & { return io_context_; }

private:
  static boost::asio::io_context io_context_;
  static boost::asio::steady_timer timer_;
};

}; // namespace Async
