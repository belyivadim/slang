#include <iostream>
#include <unordered_map>

#include "ICallable.hpp"
#include "SlangClass.hpp"
#include "SlangInstance.hpp"
#include "InterpreterExceptions.hpp"
#include "Interpreter.hpp"
#include "SlangFn.hpp"
#include "native_fn/Clock.hpp"


namespace slang {

// ------------------------ | HELPERS |
static void check_number_operand(const Token& operator_,
                                 const Object& operand) {
  if (std::holds_alternative<double>(operand)) return;

  throw RuntimeError(operator_, "Operand must be a number.");
}

static void check_number_operands(const Token& operator_,
                                 const Object& left,
                                 const Object& right) {
  if (std::holds_alternative<double>(left)
      && std::holds_alternative<double>(right)) return;

  throw RuntimeError(operator_, "Operands must be numbers.");
}

// ------------------------ | PUBLIC |
Interpreter::Interpreter(std::shared_ptr<ErrorReporter> reporter)
  : m_reporter(reporter),
    m_global(std::make_unique<Environment>(Environment{})),
    m_env(m_global.get())
{
  m_global->define("clock", 
                   std::make_shared<native_fn::Clock>(native_fn::Clock{}));
}


void Interpreter::interpret(vector<shared_ptr<stmt::Stmt>>& statements) {
  try {
    for (auto& s : statements) {
      execute(*s);
    }
  } catch (const RuntimeError& e) {
    m_reporter->runtime_error(e);
  }
}

void Interpreter::resolve(expr::Expr& expr, int depth) {
  m_locals.insert({&expr, depth});
}

void Interpreter::visitUnaryExpr(expr::Unary &expr) {
  Object right = evaluate(*expr.m_right);

  switch (expr.m_oper.m_type) {
    case MINUS:
      check_number_operand(expr.m_oper, right);
      Return(-std::get<double>(right));   
      break;
    case BANG:
      Return(!is_truthy(right));
      break;
    default:
      Return(nullptr);
      break;
  }
}


void Interpreter::visitBinaryExpr(expr::Binary &expr) {
  Object left = evaluate(*expr.m_left);
  Object right = evaluate(*expr.m_right);

  switch (expr.m_oper.m_type) {
    case GREATER:
      check_number_operands(expr.m_oper, left, right);
      Return(std::get<double>(left) > std::get<double>(right));
      break;
    case GREATER_EQ:
      check_number_operands(expr.m_oper, left, right);
      Return(std::get<double>(left) >= std::get<double>(right));
      break;
    case LESS:
      check_number_operands(expr.m_oper, left, right);
      Return(std::get<double>(left) < std::get<double>(right));
      break;
    case LESS_EQ:
      check_number_operands(expr.m_oper, left, right);
      Return(std::get<double>(left) <= std::get<double>(right));
      break;

    case BANG_EQ:
      Return(left != right);
      break;
    case EQ_EQ:
      Return(left == right);
      break;

    case SLASH:
      check_number_operands(expr.m_oper, left, right);
      Return(std::get<double>(left) / std::get<double>(right));
      break;
    case STAR:
      check_number_operands(expr.m_oper, left, right);
      Return(std::get<double>(left) * std::get<double>(right));
      break;
    case MINUS:
      check_number_operands(expr.m_oper, left, right);
      Return(std::get<double>(left) - std::get<double>(right));
      break;
    case PLUS:
      if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
        Return(std::get<double>(left) + std::get<double>(right));
        break;
      } else if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right)) {
        Return(std::get<std::string>(left) + std::get<std::string>(right));
        break;
      }
      throw RuntimeError(expr.m_oper,
                        "Operands must be two numbers or two strings.");

    default:
      Return(nullptr);
      break;
  }
}

void Interpreter::visitLiteralExpr(expr::Literal &expr) {
  Return(expr.m_value);
}

void Interpreter::visitGroupingExpr(expr::Grouping &expr) {
  Return(evaluate(*expr.m_expression));
}

void Interpreter::visitVariableExpr(expr::Variable &expr) {
  Return(lookup_variable(expr.m_name, expr));
}

void Interpreter::visitAssignExpr(expr::Assign &expr) {
  auto value = evaluate(*expr.m_value);

  auto distance = m_locals.find(&expr);
  if (distance != m_locals.end()) {
    m_env->assign_at(distance->second, expr.m_name, value);
  } else {
    m_global->assign(expr.m_name, value);
  }

  m_env->assign(expr.m_name, value);
  Return(value);
}

void Interpreter::visitLogicalExpr(expr::Logical &expr) {
  auto left = evaluate(*expr.m_left);

  if (expr.m_oper.m_type == OR) {
    if (is_truthy(left)) {
      Return(left);
      return;
    }
  } else {
    if (!is_truthy(left)) {
      Return(left);
      return;
    }
  }

  Return(evaluate(*expr.m_right));
}

void Interpreter::visitCallExpr(expr::Call &expr){
  auto callee = evaluate(*expr.m_callee);

  ICallable *fn = nullptr;
  if (auto *f = std::get_if<shared_ptr<ICallable>>(&callee)) {
    fn = f->get();
  } else if (ICallable **f = std::get_if<ICallable*>(&callee)) {
    fn = *f;
  }

  if (fn != nullptr) {
    if (expr.m_args.size() != fn->arity()) {
      throw RuntimeError(expr.m_paren, "Expected " + 
                         std::to_string(fn->arity()) + " arguments, but got " + 
                         std::to_string(expr.m_args.size()) + ".");
    }

    vector<Object> args;
    for (auto& arg : expr.m_args) {
      args.push_back(evaluate(*arg));
    }

    Return(fn->call(*this, args));
  } else {
    throw RuntimeError(expr.m_paren, "Can only call functions.");
  }
} 

void Interpreter::visitGetExpr(expr::Get &expr) {
  auto obj = evaluate(*expr.m_object);
  
  if (auto pobj = std::get_if<shared_ptr<SlangInstance>>(&obj)) {
    Return(pobj->get()->get_property(expr.m_name));
  } else {
    throw RuntimeError(expr.m_name, "Only instances have properties.");
  }
}

void Interpreter::visitSetExpr(expr::Set &expr) {
  auto obj = evaluate(*expr.m_object);

  if (auto pobj = std::get_if<shared_ptr<SlangInstance>>(&obj)) {
    auto value = evaluate(*expr.m_value);
    pobj->get()->set_property(expr.m_name, value);
    Return(value);
  } else {
    throw RuntimeError(expr.m_name, "Only instances have fields.");
  }
}

void Interpreter::visitExpressionStmt(stmt::Expression &stmt) {
  evaluate(*stmt.m_expression);
}

void Interpreter::visitPrintStmt(stmt::Print &stmt) {
  auto value = evaluate(*stmt.m_expression);
  std::cout << object_to_string(value) << std::endl;
}

void Interpreter::visitVarStmt(stmt::Var& stmt) {
  Object value = nullptr;
  if (stmt.m_initializer != nullptr) {
    value = evaluate(*stmt.m_initializer);
  }

  m_env->define(stmt.m_name.m_lexeme, value);
}


void Interpreter::visitBlockStmt(stmt::Block &stmt) {
  auto env = std::make_unique<Environment>(Environment(m_env));
  executeBlock(stmt.m_statements, env.get());
}

void Interpreter::visitIfStmt(stmt::If &stmt) {
  if (is_truthy(evaluate(*stmt.m_condition))) {
    execute(*stmt.m_then_branch);
  } else if (stmt.m_else_branch != nullptr) {
    execute(*stmt.m_else_branch);
  }
}


void Interpreter::visitWhileStmt(stmt::While &stmt) {
  if (is_truthy(evaluate(*stmt.m_condition))) {
    do {
      try {
        execute(*stmt.m_then_branch);
      } catch (const BreakExc&) {
        break;
      }
    } while (is_truthy(evaluate(*stmt.m_condition)));
  } else if (stmt.m_else_branch != nullptr) {
    execute(*stmt.m_else_branch);
  }
}


void Interpreter::visitFnStmt(stmt::Fn &stmt) {
  // make copy of env and store it in closure
  auto closure = std::make_unique<Environment>(Environment(*m_env));
  auto fn = std::make_shared<SlangFn>(SlangFn(stmt, std::move(closure))); 
  m_env->define(stmt.m_name.m_lexeme, fn);
}

void Interpreter::visitReturnStmt(stmt::Return &stmt) {
  Object ret = nullptr;
  if (stmt.m_value != nullptr) {
    ret = evaluate(*stmt.m_value);
  }

  throw ReturnExc(ret);
}


void Interpreter::visitBreakStmt(stmt::Break &) {
  throw BreakExc{};
}

void Interpreter::visitClassStmt(stmt::Class &stmt) {
  m_env->define(stmt.m_name.m_lexeme, nullptr);

  std::unordered_map<string, shared_ptr<SlangFn>> methods;
  for (auto& method : stmt.m_methods) {
    auto closure = std::make_unique<Environment>(Environment(*m_env));
    auto fn = make_shared<SlangFn>(SlangFn(*method, std::move(closure)));
    methods.insert({method->m_name.m_lexeme, fn});
  }

  auto cls = std::make_shared<SlangClass>(SlangClass(stmt.m_name.m_lexeme, methods));
  m_env->assign(stmt.m_name, cls);
}

// ------------------------ | PRIVATE |
Object Interpreter::evaluate(expr::Expr& expr) {
  return GetValue(expr);
}

bool Interpreter::is_truthy(const Object& obj) {
  if (std::holds_alternative<std::nullptr_t>(obj)
      || (std::holds_alternative<double>(obj) && std::get<double>(obj) == 0)) {
    return false;
  }

  if (std::holds_alternative<bool>(obj)) {
    return std::get<bool>(obj);
  }

  return true;
}

void Interpreter::execute(stmt::Stmt& statement) {
  statement.accept(*this);
}

void Interpreter::executeBlock(
    vector<shared_ptr<stmt::Stmt>>& statements,
    Environment *env
) {

  /// Stores environment @env and restore it on destruction
  class RaiiEnv {
  public:
    explicit RaiiEnv(Environment** env)
      : m_env_to_restore(env),
        m_prev(*env) {}

    ~RaiiEnv() {
      *m_env_to_restore = m_prev;
    }

  private:
    Environment** m_env_to_restore;
    Environment* m_prev;
  };


  RaiiEnv e(&m_env);
  m_env = env;
  for (auto& s : statements) {
    execute(*s);
  }
}


Object Interpreter::lookup_variable(const Token& name, expr::Expr& expr) {
  auto distance = m_locals.find(&expr);

  if (distance != m_locals.end()) {
    return m_env->get_variable_at(distance->second, name.m_lexeme);
  } else {
    return m_global->get_variable(name);
  }
}


} // namespace slang
