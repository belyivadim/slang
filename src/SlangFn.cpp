#include "SlangFn.hpp"
#include "InterpreterExceptions.hpp"

namespace slang {


SlangFn::SlangFn(stmt::Fn& declaration)
  : m_declaration(declaration) {}


Object SlangFn::call(Interpreter &interpreter, std::vector<Object> &args) {
  auto env = std::make_unique<Environment>(
    Environment(interpreter.get_global_environment()));
  for (size_t i = 0; i < m_declaration.m_params.size(); ++i) {
    env->define(m_declaration.m_params[i].m_lexeme, args[i]);
  }

  try {
    interpreter.executeBlock(m_declaration.m_body, env.get());
  } catch (ReturnExc& ret) {
    return ret.m_value;
  }

  return nullptr;
}

size_t SlangFn::arity() {
  return m_declaration.m_params.size();
}

std::string SlangFn::to_string() const {
  return "<fn " + m_declaration.m_name.m_lexeme + ">";
}

} // namespace slang
