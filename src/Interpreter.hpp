#ifndef __SLANG_INTERPRETER_HPP__
#define __SLANG_INTERPRETER_HPP__

#include <stdexcept>
#include <vector>

#include "Environment.hpp"
#include "Expr.hpp"
#include "Stmt.hpp"
#include "ErrorReporter.hpp"

namespace slang {

using std::vector;
using std::shared_ptr;
using std::unique_ptr;

class Interpreter : public expr::ValueGetter<Interpreter, expr::Expr, Object>,
                    public expr::IVisitor,
                    public stmt::IVisitor {
public:
  Interpreter(std::shared_ptr<ErrorReporter> reporter);
  Interpreter(Interpreter &&) = default;
  Interpreter(const Interpreter &) = delete;
  Interpreter &operator=(Interpreter &&) = default;
  Interpreter &operator=(const Interpreter &) = delete;
  ~Interpreter() = default;

  void visitUnaryExpr(expr::Unary &expr) override;
  void visitBinaryExpr(expr::Binary &expr) override;
  void visitLiteralExpr(expr::Literal &expr) override;
  void visitGroupingExpr(expr::Grouping &expr) override;
  void visitVariableExpr(expr::Variable &expr) override;
  void visitAssignExpr(expr::Assign &expr) override;
  void visitLogicalExpr(expr::Logical &expr) override;

  void visitExpressionStmt(stmt::Expression &stmt) override;
  void visitPrintStmt(stmt::Print &stmt) override;
  void visitVarStmt(stmt::Var &stmt) override;
  void visitBlockStmt(stmt::Block &stmt) override;
  void visitIfStmt(stmt::If &stmt) override;

  void interpret(vector<shared_ptr<stmt::Stmt>>& statements);
private:
  shared_ptr<ErrorReporter> m_reporter;
  unique_ptr<Environment> m_env;

  Object evaluate(expr::Expr& expr);
  bool is_truthy(const Object& obj);
  
  void execute(stmt::Stmt& statement);
  void executeBlock(vector<shared_ptr<stmt::Stmt>>& statements,
                    unique_ptr<Environment> env);

};

} // namespace slang

#endif // !__SLANG_INTERPRETER_HPP__
