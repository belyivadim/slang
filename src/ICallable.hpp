#ifndef __SLANG_ICALLABLE_HPP__
#define __SLANG_ICALLABLE_HPP__

#include <string>
#include <vector>

#include "Object.hpp"
#include "Interpreter.hpp"


namespace slang {

class ICallable {
public:
  ICallable() = default;
  ICallable(ICallable &&) = default;
  ICallable(const ICallable &) = default;
  ICallable &operator=(ICallable &&) = default;
  ICallable &operator=(const ICallable &) = default;
  virtual ~ICallable() = default;

  virtual Object call(Interpreter& interpreter, std::vector<Object>& args) = 0;

  virtual size_t arity() = 0;

  virtual std::string to_string() const {
    return "<fn @" + std::to_string(size_t(this)) + ">";
  }

};

} // namespace slang

#endif // !__SLANG_ICALLABLE_HPP__
