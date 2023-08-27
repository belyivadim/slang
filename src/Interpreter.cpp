#include <iostream>

#include "RuntimeError.hpp"
#include "Interpreter.hpp"


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
  m_env(std::make_unique<Environment>(Environment{})) 
{}


void Interpreter::interpret(vector<shared_ptr<stmt::Stmt>>& statements) {
  try {
    for (auto& s : statements) {
      execute(*s);
    }
  } catch (const RuntimeError& e) {
    m_reporter->runtime_error(e);
  }
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
  Return(m_env->get_variable(expr.m_name));
}

void Interpreter::visitAssignExpr(expr::Assign &expr) {
  auto value = evaluate(*expr.m_value);
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
  executeBlock(stmt.m_statements,
               std::make_unique<Environment>(Environment(&*m_env)));
}

void Interpreter::visitIfStmt(stmt::If &stmt) {
  if (is_truthy(evaluate(*stmt.m_condition))) {
    execute(*stmt.m_then_branch);
  } else if (stmt.m_else_branch != nullptr) {
    execute(*stmt.m_else_branch);
  }
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
    std::unique_ptr<Environment> env
) {

  /// Stores environment @env and restore it on destruction
  class RaiiEnv {
  public:
    explicit RaiiEnv(unique_ptr<Environment>& env)
      : m_env_to_restore(env),
        m_prev(std::move(env)) {}

    ~RaiiEnv() {
      m_env_to_restore = std::move(m_prev);
    }

  private:
    unique_ptr<Environment>& m_env_to_restore;
    unique_ptr<Environment> m_prev;
  };


  RaiiEnv e(m_env);
  m_env = std::move(env);
  for (auto& s : statements) {
    execute(*s);
  }
}


} // namespace slang
