#pragma once

// Pull in the reference implementation of P2300:
#include "Utils/Logger.hpp"
#include <exception>
#include <stdexec/execution.hpp>

template <class From, class To>
using _copy_cvref_t = stdexec::__copy_cvref_t<From, To>;

template <class From, class To>
concept _decays_to = std::same_as<std::decay_t<From>, To>;

///////////////////////////////////////////////////////////////////////////////
// retry algorithm:

// _conv needed so we can emplace construct non-movable types into
// a std::optional.
template <std::invocable F>
  requires std::is_nothrow_move_constructible_v<F>
struct _conv {
  F f_;

  explicit _conv(F f) noexcept : f_(static_cast<F &&>(f)) {}

  operator std::invoke_result_t<F>() && { return static_cast<F &&>(f_)(); }
};

// 增加 Duration 模板参数
template <class S, class R, class Duration> struct _op;

// 只捕获set_error，传递其他信号
// 增加 Duration 模板参数
template <class S, class R, class Duration>
struct _retry_receiver
    : stdexec::receiver_adaptor<_retry_receiver<S, R, Duration>> {
  _op<S, R, Duration> *o_; // 指向新的 _op 类型

  auto base() && noexcept -> R && { return static_cast<R &&>(o_->r_); }

  auto base() const & noexcept -> const R & { return o_->r_; }

  explicit _retry_receiver(_op<S, R, Duration> *o) : o_(o) {}

  void set_error(std::exception_ptr &&ec) && noexcept {
    try {
      std::rethrow_exception(ec);
    } catch (const std::exception &e) {
      log(error, "{}", e.what());
    }
    // 从 o_ 直接获取 duration_
    log(warning, "Retrying operation in {} ms",
        std::chrono::duration_cast<std::chrono::milliseconds>(o_->duration_)
            .count());
    o_->_retry(); // This causes the op to be retried
  }
};

// Hold the nested operation state in an optional so we can
// re-construct and re-start it if the operation fails.
// 增加 Duration 模板参数
template <class S, class R, class Duration> struct _op {
  S s_;
  R r_;
  Duration duration_; // 添加 duration_ 成员

  // 更新嵌套的 operation state 类型
  std::optional<stdexec::connect_result_t<S &, _retry_receiver<S, R, Duration>>>
      o_;

  // 构造函数接收 duration
  _op(S s, R r, Duration duration)
      : s_(static_cast<S &&>(s)), r_(static_cast<R &&>(r)), duration_(duration),
        o_{_connect()} {}

  _op(_op &&) = delete;

  auto _connect() noexcept {
    return _conv{[this] {
      return stdexec::connect(s_, _retry_receiver<S, R, Duration>{this});
    }};
  }

  void _retry() noexcept {
    STDEXEC_TRY {
      // 直接使用成员 duration_
      std::this_thread::sleep_for(duration_);
      o_.emplace(_connect()); // 在原地重新构建！
      stdexec::start(*o_);
    }
    STDEXEC_CATCH_ALL {
      stdexec::set_error(static_cast<R &&>(r_), std::current_exception());
    }
  }

  void start() & noexcept { stdexec::start(*o_); }
};

template <class S, class Duration> struct _retry_sender {
  using sender_concept = stdexec::sender_t;
  S s_;
  Duration duration_;

  explicit _retry_sender(S s, Duration duration)
      : s_(static_cast<S &&>(s)), duration_(duration) {}

  template <class> using _error = stdexec::completion_signatures<>;
  template <class... Ts>
  using _value = stdexec::completion_signatures<stdexec::set_value_t(Ts...)>;

  template <class Env>
  auto get_completion_signatures(Env &&) const
      -> stdexec::transform_completion_signatures_of<
          S &, Env,
          stdexec::completion_signatures<
              stdexec::set_error_t(std::exception_ptr)>,
          _value, _error> {
    return {};
  }

  template <stdexec::receiver R>
  friend auto tag_invoke(stdexec::connect_t, _retry_sender &&self, R r)
      // 返回新的 _op 类型
      -> _op<S, R, Duration> {
    // 将 self.duration_ 传递给 _op 的构造函数
    return {static_cast<S &&>(self.s_), static_cast<R &&>(r), self.duration_};
  }

  auto get_env() const noexcept -> stdexec::env_of_t<S> {
    return stdexec::get_env(s_);
  }
};

template <stdexec::sender S, class Duration>
auto retry(S s, const Duration &duration) -> stdexec::sender auto {
  return _retry_sender{static_cast<S &&>(s), duration};
}
