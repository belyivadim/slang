#include "SlangClass.hpp"
#include "SlangInstance.hpp"

namespace slang {

SlangClass::SlangClass(const string& name,
                       const std::unordered_map<string, shared_ptr<SlangFn>>& methods)
  : m_name(name),
    m_methods(methods)
{}

string SlangClass::to_string() const {
  return "class <" + m_name + ">";
}

Object SlangClass::call(Interpreter &interpreter, std::vector<Object> &args) {
  auto instance = make_shared<SlangInstance>(SlangInstance(this));
  return instance;
}

size_t SlangClass::arity() {
  return 0;
}


optional<shared_ptr<SlangFn>> SlangClass::find_method(const string& name) const {
  auto found = m_methods.find(name);
  if (found != m_methods.end()) {
    return found->second;
  }

  return std::nullopt;
}


} // namespace slang
