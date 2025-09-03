#include <stdexec/execution.hpp>

struct MySender {
  using sender_concept = stdexec::sender_t;
  using completion_signatures =
      stdexec::completion_signatures<stdexec::set_value_t(int),
                                     stdexec::set_error_t(std::exception_ptr),
                                     stdexec::set_stopped_t()>;
  int value;

  template <stdexec::receiver Receiver>
  struct Operation {
    Receiver receiver;
    int value;

    void start() noexcept {
      stdexec::set_value(std::move(receiver), value);
    }
  };

  template <stdexec::receiver Receiver>
  // Operation<Receiver> tag_invoke(stdexec::connect_t, MySender sender, Receiver r) {
  //   return Operation<Receiver>{std::move(r), sender.value};
  // }
  Operation<Receiver> connect(Receiver r) {
    return Operation<Receiver>{std::move(r), value};
  }
};


auto main() -> int {

  auto MySender1 = MySender{42};
  stdexec::sync_wait(MySender1);
  return 0;
}
