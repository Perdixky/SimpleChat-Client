// Minimal stdexec scheduler that hops continuations onto the Node.js main
// thread using Napi::ThreadSafeFunction (TSFN).
//
// This scheduler is intended to be used with stdexec's continues_on adapter:
//   some_sender | stdexec::continues_on(js_sched) | stdexec::then([]{ ... N-API ... });
// The then-continuation will run on the JS thread because the hop is realized
// by issuing a TSFN::NonBlockingCall and calling set_value inside the callback.

#pragma once

#include <napi.h>
#include <stdexec/execution.hpp>
#include <exception>

namespace Async {

class JsScheduler {
public:
  JsScheduler() noexcept = default;

  // Construct a scheduler bound to an existing TSFN that must outlive any
  // operations started on this scheduler. We store a raw pointer because the
  // TSFN's lifetime is owned by the addon wrapper; the TSFN itself is
  // thread-safe and may be invoked from any thread.
  static JsScheduler from_tsfn(Napi::ThreadSafeFunction &tsfn) noexcept {
    JsScheduler s;
    s.tsfn_ = &tsfn;
    return s;
  }

  // Equality compares the underlying TSFN identity.
  friend bool operator==(const JsScheduler &a, const JsScheduler &b) noexcept {
    return a.tsfn_ == b.tsfn_;
  }
  friend bool operator!=(const JsScheduler &a, const JsScheduler &b) noexcept {
    return !(a == b);
  }

private:
  // Sender returned by schedule(scheduler)
  struct schedule_sender {
    Napi::ThreadSafeFunction *tsfn{};
    using sender_concept = stdexec::sender_t; // enable stdexec sender detection

    // Inform stdexec of the possible completions
    using completion_signatures = stdexec::completion_signatures<
        stdexec::set_value_t(),
        stdexec::set_error_t(std::exception_ptr),
        stdexec::set_stopped_t()>;

    template <class Receiver> struct operation {
      Napi::ThreadSafeFunction *tsfn{};
      Receiver receiver;

      // heap state used by NonBlockingCall so the receiver survives until the
      // JS-thread callback runs.
      struct state {
        Napi::ThreadSafeFunction *tsfn;
        Receiver receiver;
      };

      // Start is called on an arbitrary thread. We hop to the JS thread by
      // issuing a TSFN NonBlockingCall. Inside the JS callback we complete the
      // operation by delivering set_value to the downstream receiver; any
      // subsequent work (e.g., N-API calls in `then`) will therefore execute on
      // the JS thread.
      friend void tag_invoke(stdexec::start_t, operation &op) noexcept {
        if (!op.tsfn) {
          stdexec::set_error(std::move(op.receiver),
                             std::make_exception_ptr(
                                 std::runtime_error("JS TSFN not set")));
          return;
        }

        // Move receiver into heap state for safe lifetime with NonBlockingCall.
        auto *st = new (std::nothrow) state{op.tsfn, std::move(op.receiver)};
        if (!st) {
          stdexec::set_error(std::move(op.receiver),
                             std::make_exception_ptr(
                                 std::bad_alloc{}));
          return;
        }

        const napi_status rc = op.tsfn->NonBlockingCall(
            st, [](Napi::Env /*env*/, Napi::Function /*unused*/, state *self) {
              // Complete on JS thread then delete the heap state
              stdexec::set_value(std::move(self->receiver));
              delete self;
            });
        if (rc != napi_ok) {
          // Fallback: deliver error inline and free state
          auto rcv = std::move(st->receiver);
          delete st;
          stdexec::set_error(std::move(rcv),
                             std::make_exception_ptr(std::runtime_error(
                                 "TSFN NonBlockingCall failed")));
        }
      }
    };

    template <class Receiver>
    friend operation<Receiver>
    tag_invoke(stdexec::connect_t, schedule_sender s, Receiver r) noexcept {
      return operation<Receiver>{s.tsfn, std::move(r)};
    }

    friend auto tag_invoke(stdexec::get_env_t, const schedule_sender &s) noexcept {
      // Expose the scheduler through the environment so that
      // get_completion_scheduler<set_value_t>(get_env(schedule(s))) returns
      // this scheduler, satisfying the stdexec scheduler concept.
      JsScheduler sched;
      sched.tsfn_ = s.tsfn;
      return stdexec::__sched_attrs{sched};
    }
  };

  // Provide schedule(scheduler) customization for stdexec
  friend schedule_sender tag_invoke(stdexec::schedule_t,
                                    const JsScheduler &sch) noexcept {
    return schedule_sender{sch.tsfn_};
  }

  Napi::ThreadSafeFunction *tsfn_{};
};

} // namespace Async
