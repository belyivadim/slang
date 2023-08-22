#ifndef __SLANG_AST_PRINTER__
#define __SLANG_AST_PRINTER__

#include <vector>

#include "Expr.hpp"

namespace slang {

using std::string;
using std::vector;
using std::shared_ptr;

class AstPrinter : public expr::ValueGetter<AstPrinter, expr::Expr, string>,
                   public expr::IVisitor {
public:
  AstPrinter() = default;
  AstPrinter(AstPrinter &&) = default;
  AstPrinter(const AstPrinter &) = default;
  AstPrinter &operator=(AstPrinter &&) = default;
  AstPrinter &operator=(const AstPrinter &) = default;
  ~AstPrinter() = default;

  string print(expr::Expr& expr) {
    return GetValue(expr);
  }

  void visitUnaryExpr(expr::Unary &expr) override {
    Return(parenthesize(expr.m_oper.m_lexeme, {expr.m_right}));
  }

  void visitBinaryExpr(expr::Binary &expr) override {
    Return(parenthesize(expr.m_oper.m_lexeme, {expr.m_left, expr.m_right}));
  }

  void visitGroupingExpr(expr::Grouping &expr) override {
    Return(parenthesize("group", {expr.m_expression}));
  }

  void visitLiteralExpr(expr::Literal &expr) override {
    Return(Token::literal_to_string(expr.m_value));
  }
  
private:

  string parenthesize(const string& name, const vector<std::shared_ptr<expr::Expr>>& exprs) {
    string result = "(" + name;

    for (auto e : exprs) {
      result.append(" " + GetValue(*e));
    }
    
    result.append(")");

    return result;
  }
  
};

} // namespace slang

#endif // __SLANG_AST_PRINTER__
