#ifndef __SLANG_INTERPRETER_HPP__
#define __SLANG_INTERPRETER_HPP__

#include "Expr.hpp"

namespace slang {

class Interpreter : public expr::ValueGetter<Interpreter, expr::Expr, Object>,
                    public expr::IVisitor {
public:
  Interpreter() = default;
  Interpreter(Interpreter &&) = default;
  Interpreter(const Interpreter &) = default;
  Interpreter &operator=(Interpreter &&) = default;
  Interpreter &operator=(const Interpreter &) = default;
  ~Interpreter() = default;

  void visitUnaryExpr(expr::Unary &expr) override;
  void visitBinaryExpr(expr::Binary &expr) override;
  void visitLiteralExpr(expr::Literal &expr) override;
  void visitGroupingExpr(expr::Grouping &expr) override;

  Object evaluate(expr::Expr& expr);
private:

  bool is_truthy(const Object& obj);
  
};

} // namespace slang

#endif // !__SLANG_INTERPRETER_HPP__
