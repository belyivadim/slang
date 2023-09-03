#include "SlangInstance.hpp"

namespace slang {

string SlangInstance::to_string() const {
  return "instance of " + m_cls->to_string();
}

Object SlangInstance::get_property(const Token& name) {
  auto field = m_fields.find(name.m_lexeme); 
  if (field != m_fields.end()) {
    return field->second;
  }

  auto method = m_cls->find_method(name.m_lexeme);

  if (method.has_value()) {
    return method.value();
  }

  throw RuntimeError(name, "Undefined get_property '" + name.m_lexeme + "'.");
}


void SlangInstance::set_property(const Token& name, const Object& value) {
  m_fields.insert_or_assign(name.m_lexeme, value);
}
  
} // namespace slang
