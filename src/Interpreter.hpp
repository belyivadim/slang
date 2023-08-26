#ifndef __SLANG_INTERPRETER_HPP__
#define __SLANG_INTERPRETER_HPP__

#include <stdexcept>

#include "Expr.hpp"
#include "ErrorReporter.hpp"

namespace slang {

class Interpreter : public expr::ValueGetter<Interpreter, expr::Expr, Object>,
                    public expr::IVisitor {
public:
  Interpreter(std::shared_ptr<ErrorReporter> reporter);
  Interpreter(Interpreter &&) = default;
  Interpreter(const Interpreter &) = default;
  Interpreter &operator=(Interpreter &&) = default;
  Interpreter &operator=(const Interpreter &) = default;
  ~Interpreter() = default;

  void visitUnaryExpr(expr::Unary &expr) override;
  void visitBinaryExpr(expr::Binary &expr) override;
  void visitLiteralExpr(expr::Literal &expr) override;
  void visitGroupingExpr(expr::Grouping &expr) override;

  void interpret(expr::Expr& expr);
private:
  std::shared_ptr<ErrorReporter> m_reporter;

  Object evaluate(expr::Expr& expr);
  bool is_truthy(const Object& obj);
  


public:
};

} // namespace slang

#endif // !__SLANG_INTERPRETER_HPP__
