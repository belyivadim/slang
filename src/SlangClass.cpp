#include "SlangClass.hpp"
#include "SlangInstance.hpp"

namespace slang {

SlangClass::SlangClass(const string& name)
  : m_name(name) {}

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


} // namespace slang
