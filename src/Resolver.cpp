#include "Resolver.hpp"

namespace slang {

// ------------------------ | PUBLIC |
Resolver::Resolver(Interpreter& interpreter,
                   shared_ptr<ErrorReporter>& reporter)
  : m_interpreter(interpreter),
    m_reporter(reporter) 
{}


void Resolver::visitBlockStmt(stmt::Block &stmt) {
  begin_scope();
  resolve(stmt.m_statements);
  end_scope();
}

void Resolver::visitVarStmt(stmt::Var &stmt) {
  declare(stmt.m_name);
  if (stmt.m_initializer != nullptr) {
    resolve(*stmt.m_initializer);
  }
  define(stmt.m_name);
}

void Resolver::visitFnStmt(stmt::Fn &stmt) {
  declare(stmt.m_name);
  define(stmt.m_name);

  resolve_function(stmt, FN_FUNCTION);
}

void Resolver::visitExpressionStmt(stmt::Expression &stmt) {
  resolve(*stmt.m_expression);
}

void Resolver::visitIfStmt(stmt::If &stmt) {
  resolve(*stmt.m_condition);
  resolve(*stmt.m_then_branch);
  if (stmt.m_else_branch != nullptr) resolve(*stmt.m_else_branch);
}

void Resolver::visitPrintStmt(stmt::Print &stmt) {
  if (stmt.m_expression != nullptr) {
    resolve(*stmt.m_expression);
  }
}

void Resolver::visitReturnStmt(stmt::Return &stmt) {
  if (m_current_fn == FN_NONE) {
    m_reporter->error(stmt.m_keyword, "Can't return from top level code.");
  }

  if (stmt.m_value != nullptr) {
    resolve(*stmt.m_value);
  }
}

void Resolver::visitWhileStmt(stmt::While &stmt) {
  resolve(*stmt.m_condition);
  m_is_break_allowed = true;
  resolve(*stmt.m_then_branch);
  m_is_break_allowed = false;
  if (stmt.m_else_branch != nullptr) resolve(*stmt.m_else_branch);
}

void Resolver::visitBreakStmt(stmt::Break &stmt) {
  if (m_is_break_allowed == false) {
    m_reporter->error(stmt.m_keyword, "break is not allowed here.");
  }
}

void Resolver::visitClassStmt(stmt::Class &stmt) {
  declare(stmt.m_name);
  define(stmt.m_name);

  for (auto& method : stmt.m_methods) {
    auto declaration = FN_METHOD;
    resolve_function(*method, declaration);
  }
}

void Resolver::visitVariableExpr(expr::Variable &expr) {
  if (!m_scopes.empty()) {
    for (auto it = m_scopes.rbegin(); it != m_scopes.rend(); ++it) {
      auto found = it->find(expr.m_name.m_lexeme);
      
      if (found != it->end() && found->second == false) {
        m_reporter->error(expr.m_name, 
                          "Can't read local variable in its own initializer.");
        return;
      }
    }
  } 

  resolve_local(expr, expr.m_name);
}


void Resolver::visitAssignExpr(expr::Assign &expr) {
  resolve(*expr.m_value);
  resolve_local(expr, expr.m_name);
}

void Resolver::visitBinaryExpr(expr::Binary &expr) {
  resolve(*expr.m_left);
  resolve(*expr.m_right);
}

void Resolver::visitCallExpr(expr::Call &expr) {
  resolve(*expr.m_callee);

  for (auto& arg : expr.m_args) {
    resolve(*arg);
  }
}

void Resolver::visitGroupingExpr(expr::Grouping &expr) {
  resolve(*expr.m_expression);
}

void Resolver::visitLiteralExpr(expr::Literal &) {
  // empry body
}

void Resolver::visitLogicalExpr(expr::Logical &expr) {
  resolve(*expr.m_left);
  resolve(*expr.m_right);
}

void Resolver::visitUnaryExpr(expr::Unary &expr) {
  resolve(*expr.m_right);
}


void Resolver::visitGetExpr(expr::Get &expr) {
  resolve(*expr.m_object);
}

void Resolver::visitSetExpr(expr::Set &expr) {
  resolve(*expr.m_value);
  resolve(*expr.m_object);
}


// ------------------------ | PRIVATE |
void Resolver::resolve(vector<shared_ptr<stmt::Stmt>>& statements) {
  for (auto& s : statements) {
    resolve(*s);
  }
}

void Resolver::resolve(stmt::Stmt& stmt) {
  stmt.accept(*this);
}

void Resolver::resolve(expr::Expr& expr) {
  expr.accept(*this);
}

void Resolver::resolve_local(expr::Expr& expr, const Token& name) {
  for (auto it = m_scopes.rbegin(); it != m_scopes.rend(); ++it) {
    if (it->find(name.m_lexeme) != it->end()) {
      m_interpreter.resolve(expr, abs(int(m_scopes.rbegin() - it)));
      return;
    }
  }
}

void Resolver::resolve_function(stmt::Fn& fn, FnType fn_type) {
  FnType enclosing_fn = m_current_fn;
  m_current_fn = fn_type;

  begin_scope();

  for (auto& param : fn.m_params) {
    declare(param);
    define(param);
  }
  resolve(fn.m_body);

  end_scope();

  m_current_fn = enclosing_fn;
}

void Resolver::begin_scope() {
  m_scopes.push_back(unordered_map<string, bool>{});
}

void Resolver::end_scope() {
  m_scopes.pop_back();
}

void Resolver::declare(const Token& name) {
  if (m_scopes.empty()) return;

  auto& scope = m_scopes.back();

  if (scope.find(name.m_lexeme) != scope.end()) {
    m_reporter->error(name, "Already variable with this name in this scope.");
  }

  scope.insert_or_assign(name.m_lexeme, false);
}

void Resolver::define(const Token& name) {
  if (m_scopes.empty()) return;

  auto& scope = m_scopes.back();
  scope.insert_or_assign(name.m_lexeme, true);
}


} // namespace slang
