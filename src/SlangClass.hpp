#ifndef __SLANG_CLASS_HPP__
#define __SLANG_CLASS_HPP__

#include <string>

#include "Object.hpp"
#include "ICallable.hpp"

namespace slang {

using std::string;
using std::make_shared;

class SlangClass : public ICallable {
public:
  SlangClass(const string& name);

  SlangClass(SlangClass &&) = default;
  SlangClass(const SlangClass &) = default;
  SlangClass &operator=(SlangClass &&) = default;
  SlangClass &operator=(const SlangClass &) = default;
  ~SlangClass() = default;

  string to_string() const override;

  Object call(Interpreter &interpreter, std::vector<Object> &args) override;

  size_t arity() override;

private:
  string m_name;
};

inline bool operator==(const SlangClass& lhs, const SlangClass& rhs) {
  return lhs.to_string() == rhs.to_string();
}

inline bool operator!=(const SlangClass& lhs, const SlangClass& rhs) {
  return lhs.to_string() != rhs.to_string();
}

} // namespace slang

#endif // !__SLANG_CLASS_HPP__
