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
using std::unordered_map;
using std::shared_ptr;
using std::unique_ptr;

class Interpreter : public expr::ValueGetter<Interpreter, expr::Expr, Object>,
                    public expr::IVisitor,
                    public stmt::IVisitor {
public:
  Interpreter(std::shared_ptr<ErrorReporter> reporter);
  Interpreter(Interpreter &&) = default;
  Interpreter(const Interpreter &) = delete;
  Interpreter &operator=(Interpreter &&) = delete;
  Interpreter &operator=(const Interpreter &) = delete;
  ~Interpreter() = default;

  void visitUnaryExpr(expr::Unary &expr) override;
  void visitBinaryExpr(expr::Binary &expr) override;
  void visitLiteralExpr(expr::Literal &expr) override;
  void visitGroupingExpr(expr::Grouping &expr) override;
  void visitVariableExpr(expr::Variable &expr) override;
  void visitAssignExpr(expr::Assign &expr) override;
  void visitLogicalExpr(expr::Logical &expr) override;
  void visitCallExpr(expr::Call &expr) override;
  void visitGetExpr(expr::Get &expr) override;
  void visitSetExpr(expr::Set &expr) override;

  void visitExpressionStmt(stmt::Expression &stmt) override;
  void visitPrintStmt(stmt::Print &stmt) override;
  void visitVarStmt(stmt::Var &stmt) override;
  void visitBlockStmt(stmt::Block &stmt) override;
  void visitBreakStmt(stmt::Break &stmt) override;
  void visitClassStmt(stmt::Class &stmt) override;
  void visitIfStmt(stmt::If &stmt) override;
  void visitWhileStmt(stmt::While &stmt) override;
  void visitFnStmt(stmt::Fn &stmt) override;
  void visitReturnStmt(stmt::Return &stmt) override;

  void interpret(vector<shared_ptr<stmt::Stmt>>& statements);
  Environment* get_global_environment() { return m_global.get(); }

  void executeBlock(vector<shared_ptr<stmt::Stmt>>& statements,
                    Environment *env);

  void resolve(expr::Expr& expr, int depth);

private:
  shared_ptr<ErrorReporter> m_reporter;

  unique_ptr<Environment> m_global;
  Environment* m_env;

  unordered_map<expr::Expr*, int> m_locals{};


  Object evaluate(expr::Expr& expr);
  bool is_truthy(const Object& obj);
  
  void execute(stmt::Stmt& statement);

  Object lookup_variable(const Token& name, expr::Expr& expr);

};

} // namespace slang

#endif // !__SLANG_INTERPRETER_HPP__
