#include "Interpreter.hpp"


namespace slang {

// ------------------------ | PUBLIC |
void Interpreter::visitUnaryExpr(expr::Unary &expr) {
  Object right = evaluate(*expr.m_right);

  switch (expr.m_oper.m_type) {
    case MINUS:
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
      Return(std::get<double>(left) > std::get<double>(right));
      break;
    case GREATER_EQ:
      Return(std::get<double>(left) >= std::get<double>(right));
      break;
    case LESS:
      Return(std::get<double>(left) < std::get<double>(right));
      break;
    case LESS_EQ:
      Return(std::get<double>(left) <= std::get<double>(right));
      break;

    case BANG_EQ:
      Return(left != right);
      break;
    case EQ_EQ:
      Return(left == right);
      break;

    case SLASH:
      Return(std::get<double>(left) / std::get<double>(right));
      break;
    case STAR:
      Return(std::get<double>(left) * std::get<double>(right));
      break;
    case MINUS:
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
      break;

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
