#pragma once
#include "stdexec/__detail/__start_detached.hpp"
#include <boost/asio.hpp>
#include <boost/system/detail/error_code.hpp>
#include <concepts>
#include <print>
#include <stdexec/execution.hpp>

namespace Async{
class Loop {
public:
  Loop()
      : io_context_(), work_guard_(boost::asio::make_work_guard(io_context_)),
        timer_(io_context_) {}

  template <std::invocable Func> auto call_soon(const Func &&func) -> void {
    boost::asio::post(io_context_, std::forward<Func>(func));
  }

  template <stdexec::sender S> auto submit(S &&sender) -> void {
    stdexec::start_detached(std::forward<S>(sender));
  }

  template <std::invocable Func, typename Duration>
  void startTimer(Func &&func, const Duration &interval) {
    std::function<void(const boost::system::error_code &)> handle_timeout;

    handle_timeout = [this, func = std::forward<Func>(func), interval,
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

  auto cancelTimer() -> void { timer_.cancel(); }

  auto run() -> void { io_context_.run(); }

  auto getIOContext() -> boost::asio::io_context & {
    return io_context_;
  }

private:
  boost::asio::io_context io_context_;
  boost::asio::executor_work_guard<boost::asio::io_context::executor_type>
      work_guard_;
  boost::asio::steady_timer timer_;
};

extern Loop loop;
};

