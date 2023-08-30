#ifndef __SLANG_FN_HPP__
#define __SLANG_FN_HPP__

#include "ICallable.hpp"
#include "Interpreter.hpp"

namespace slang {

class SlangFn : public ICallable {
public:
  SlangFn(stmt::Fn& declaration, std::shared_ptr<Environment> closure);
  SlangFn(SlangFn &&) = default;
  SlangFn(const SlangFn &) = delete;
  SlangFn &operator=(SlangFn &&) = delete;
  SlangFn &operator=(const SlangFn &) = delete;
  ~SlangFn() = default;

  Object call(Interpreter &interpreter, std::vector<Object> &args) override;

  size_t arity() override;

  std::string to_string() const override;

private:
  stmt::Fn& m_declaration;
  std::shared_ptr<Environment> m_closure;
  
};

} // namespace slang

#endif // !__SLANG_FN_HPP__
