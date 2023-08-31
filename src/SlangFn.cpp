#include "SlangFn.hpp"
#include "InterpreterExceptions.hpp"

namespace slang {


SlangFn::SlangFn(stmt::Fn& declaration, std::unique_ptr<Environment> closure)
  : m_declaration(declaration),
    m_closure(std::move(closure))
{
  m_closure->define(m_declaration.m_name.m_lexeme, this);
}

SlangFn::SlangFn(SlangFn&& other)
  : m_declaration(other.m_declaration),
    m_closure(std::move(other.m_closure))
{
  m_closure->define(m_declaration.m_name.m_lexeme, this);
}

Object SlangFn::call(Interpreter &interpreter, std::vector<Object> &args) {
  auto env = std::make_unique<Environment>(Environment(m_closure.get()));
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
