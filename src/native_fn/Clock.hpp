#ifndef __SLANG_NATIVE_CLOCK_HPP__
#define __SLANG_NATIVE_CLOCK_HPP__

#include <chrono>
#include "../ICallable.hpp"

namespace slang {

namespace native_fn {

class Clock : public ICallable {
public:
  Clock() = default;
  Clock(Clock &&) = default;
  Clock(const Clock &) = default;
  Clock &operator=(Clock &&) = default;
  Clock &operator=(const Clock &) = default;
  ~Clock() = default;

  size_t arity() override { return 0; }

  Object call(Interpreter &interpreter, std::vector<Object> &args) override {
    (void)interpreter;
    (void)args;
    double seconds = std::chrono::system_clock::now().time_since_epoch().count();
    return seconds;
  }

  std::string to_string() const override {
    return "<native fn Clock>";
  }
};

} // namespace slang

} // namespace slang

#endif // !__SLANG_NATIVE_CLOCK_HPP__
