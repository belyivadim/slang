#ifndef __SLANG_RESOLVER_HPP__
#define __SLANG_RESOLVER_HPP__

#include <vector>
#include <unordered_map>

#include "Expr.hpp"
#include "Stmt.hpp"
#include "Interpreter.hpp"

namespace slang {

using std::unordered_map;
using std::string;
  
class Resolver : public expr::IVisitor,
                 public stmt::IVisitor {
public:
  Resolver(Interpreter& interpreter, shared_ptr<ErrorReporter>& reporter);
  Resolver(Resolver &&) = default;
  Resolver(const Resolver &) = default;
  Resolver &operator=(Resolver &&) = delete;
  Resolver &operator=(const Resolver &) = delete;
  ~Resolver() = default;

  void visitBlockStmt(stmt::Block &stmt) override;
  void visitVarStmt(stmt::Var &stmt) override;
  void visitFnStmt(stmt::Fn &stmt) override;
  void visitExpressionStmt(stmt::Expression &stmt) override;
  void visitIfStmt(stmt::If &stmt) override;
  void visitPrintStmt(stmt::Print &stmt) override;
  void visitReturnStmt(stmt::Return &stmt) override;
  void visitWhileStmt(stmt::While &stmt) override;

  void visitVariableExpr(expr::Variable &expr) override;
  void visitAssignExpr(expr::Assign &expr) override;
  void visitBinaryExpr(expr::Binary &expr) override;
  void visitCallExpr(expr::Call &expr) override;
  void visitGroupingExpr(expr::Grouping &expr) override;
  void visitLiteralExpr(expr::Literal &expr) override;
  void visitLogicalExpr(expr::Logical &expr) override;
  void visitUnaryExpr(expr::Unary &expr) override;

  void resolve(vector<shared_ptr<stmt::Stmt>>& statements);

private:
  enum FnType {
    FN_NONE, FN_FUNCTION
  };


  Interpreter& m_interpreter;
  vector<unordered_map<string, bool>> m_scopes;
  shared_ptr<ErrorReporter> m_reporter;
  FnType m_current_fn{FN_NONE};


  void resolve(stmt::Stmt& stmt);
  void resolve(expr::Expr& expr);
  void resolve_local(expr::Expr& expr, const Token& name);
  void resolve_function(stmt::Fn& fn, FnType fn_type);
  void begin_scope();
  void end_scope();
  void declare(const Token& name);
  void define(const Token& name);
  
};

} // namespace slang

#endif // !__SLANG_RESOLVER_HPP__
