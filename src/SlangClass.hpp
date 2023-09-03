#ifndef __SLANG_CLASS_HPP__
#define __SLANG_CLASS_HPP__

#include <optional>
#include <string>

#include "Object.hpp"
#include "ICallable.hpp"
#include "SlangFn.hpp"

namespace slang {

using std::string;
using std::make_shared;
using std::optional;

class SlangClass : public ICallable {
public:
  SlangClass(const string& name, 
             const std::unordered_map<string, shared_ptr<SlangFn>>& methods);

  SlangClass(SlangClass &&) = default;
  SlangClass(const SlangClass &) = default;
  SlangClass &operator=(SlangClass &&) = default;
  SlangClass &operator=(const SlangClass &) = default;
  ~SlangClass() = default;

  string to_string() const override;
  Object call(Interpreter &interpreter, std::vector<Object> &args) override;
  size_t arity() override;

  optional<shared_ptr<SlangFn>> find_method(const string& name) const;

private:
  string m_name;
  std::unordered_map<string, shared_ptr<SlangFn>> m_methods;

};

inline bool operator==(const SlangClass& lhs, const SlangClass& rhs) {
  return lhs.to_string() == rhs.to_string();
}

inline bool operator!=(const SlangClass& lhs, const SlangClass& rhs) {
  return lhs.to_string() != rhs.to_string();
}

} // namespace slang

#endif // !__SLANG_CLASS_HPP__
