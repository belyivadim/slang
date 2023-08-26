#include <iostream>

#include "RuntimeError.hpp"
#include "Interpreter.hpp"


namespace slang {


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
  : m_reporter(reporter) {}


void Interpreter::interpret(expr::Expr& expr) {
  try {
    Object value = evaluate(expr);
    std::cout << object_to_string(value) << std::endl;
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

} // namespace slang
