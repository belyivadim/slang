#ifndef __SLANG_STMT_HPP__
#define __SLANG_STMT_HPP__

#include <memory>
#include <vector>

#include "Token.hpp"
#include "Expr.hpp"

namespace slang {

namespace stmt {

class Block;
class Class;
class Break;
class Expression;
class If;
class Fn;
class Print;
class Return;
class Var;
class While;

class IVisitor {
public:
  IVisitor() = default;
  IVisitor(const IVisitor&) = default;
  IVisitor(IVisitor&&) = default;
  IVisitor& operator=(const IVisitor&) = default;
  IVisitor& operator=(IVisitor&&) = default;
  virtual ~IVisitor() = default;

  virtual void visitBlockStmt(Block& stmt) = 0;
  virtual void visitClassStmt(Class& stmt) = 0;
  virtual void visitBreakStmt(Break& stmt) = 0;
  virtual void visitExpressionStmt(Expression& stmt) = 0;
  virtual void visitIfStmt(If& stmt) = 0;
  virtual void visitFnStmt(Fn& stmt) = 0;
  virtual void visitPrintStmt(Print& stmt) = 0;
  virtual void visitReturnStmt(Return& stmt) = 0;
  virtual void visitVarStmt(Var& stmt) = 0;
  virtual void visitWhileStmt(While& stmt) = 0;
};

class Stmt {
public:
  Stmt() = default;
  Stmt(const Stmt&) = default;
  Stmt(Stmt&&) = default;
  Stmt& operator=(const Stmt&) = default;
  Stmt& operator=(Stmt&&) = default;
  virtual ~Stmt() = default;

  virtual void accept(IVisitor& visitor) = 0;

};

template <class Visitor, class Visitable, class R>
class ValueGetter {
public:
  ValueGetter() = default;
  ValueGetter(const ValueGetter&) = default;
  ValueGetter(ValueGetter&&) = default;
  ValueGetter& operator=(const ValueGetter&) = default;
  ValueGetter& operator=(ValueGetter&&) = default;
  virtual ~ValueGetter() = default;

  R& GetValue(Visitable& visitable) {
    visitable.accept(*(static_cast<Visitor*>(this)));
    return m_value;
  }

  void Return(const R& value) {
    m_value = value;
  }

private:
  R m_value;

};

class Block : public Stmt {
public:
  Block(const std::vector<std::shared_ptr<Stmt>>& statements) :
    Stmt(),
    m_statements(statements)
  {}

  Block(const Block&) = default;
  Block(Block&&) = default;
  Block& operator=(const Block&) = default;
  Block& operator=(Block&&) = default;
  virtual ~Block() = default;

  void accept(IVisitor& visitor) override {
    visitor.visitBlockStmt(*this);
  }

  std::vector<std::shared_ptr<Stmt>> m_statements;

};

class Class : public Stmt {
public:
  Class(const Token& name, const std::vector<std::shared_ptr<stmt::Fn>>& methods) :
    Stmt(),
    m_name(name),
    m_methods(methods)
  {}

  Class(const Class&) = default;
  Class(Class&&) = default;
  Class& operator=(const Class&) = default;
  Class& operator=(Class&&) = default;
  virtual ~Class() = default;

  void accept(IVisitor& visitor) override {
    visitor.visitClassStmt(*this);
  }

  Token m_name;
  std::vector<std::shared_ptr<stmt::Fn>> m_methods;

};

class Break : public Stmt {
public:
  Break(const Token& keyword) :
    Stmt(),
    m_keyword(keyword)
  {}

  Break(const Break&) = default;
  Break(Break&&) = default;
  Break& operator=(const Break&) = default;
  Break& operator=(Break&&) = default;
  virtual ~Break() = default;

  void accept(IVisitor& visitor) override {
    visitor.visitBreakStmt(*this);
  }

  Token m_keyword;

};

class Expression : public Stmt {
public:
  Expression(const std::shared_ptr<expr::Expr>& expression) :
    Stmt(),
    m_expression(expression)
  {}

  Expression(const Expression&) = default;
  Expression(Expression&&) = default;
  Expression& operator=(const Expression&) = default;
  Expression& operator=(Expression&&) = default;
  virtual ~Expression() = default;

  void accept(IVisitor& visitor) override {
    visitor.visitExpressionStmt(*this);
  }

  std::shared_ptr<expr::Expr> m_expression;

};

class If : public Stmt {
public:
  If(const std::shared_ptr<expr::Expr>& condition, const std::shared_ptr<Stmt>& then_branch, const std::shared_ptr<Stmt>& else_branch) :
    Stmt(),
    m_condition(condition),
    m_then_branch(then_branch),
    m_else_branch(else_branch)
  {}

  If(const If&) = default;
  If(If&&) = default;
  If& operator=(const If&) = default;
  If& operator=(If&&) = default;
  virtual ~If() = default;

  void accept(IVisitor& visitor) override {
    visitor.visitIfStmt(*this);
  }

  std::shared_ptr<expr::Expr> m_condition;
  std::shared_ptr<Stmt> m_then_branch;
  std::shared_ptr<Stmt> m_else_branch;

};

class Fn : public Stmt {
public:
  Fn(const Token& name, const std::vector<Token>& params, const std::vector<std::shared_ptr<Stmt>>& body) :
    Stmt(),
    m_name(name),
    m_params(params),
    m_body(body)
  {}

  Fn(const Fn&) = default;
  Fn(Fn&&) = default;
  Fn& operator=(const Fn&) = default;
  Fn& operator=(Fn&&) = default;
  virtual ~Fn() = default;

  void accept(IVisitor& visitor) override {
    visitor.visitFnStmt(*this);
  }

  Token m_name;
  std::vector<Token> m_params;
  std::vector<std::shared_ptr<Stmt>> m_body;

};

class Print : public Stmt {
public:
  Print(const std::shared_ptr<expr::Expr>& expression) :
    Stmt(),
    m_expression(expression)
  {}

  Print(const Print&) = default;
  Print(Print&&) = default;
  Print& operator=(const Print&) = default;
  Print& operator=(Print&&) = default;
  virtual ~Print() = default;

  void accept(IVisitor& visitor) override {
    visitor.visitPrintStmt(*this);
  }

  std::shared_ptr<expr::Expr> m_expression;

};

class Return : public Stmt {
public:
  Return(const Token& keyword, const std::shared_ptr<expr::Expr>& value) :
    Stmt(),
    m_keyword(keyword),
    m_value(value)
  {}

  Return(const Return&) = default;
  Return(Return&&) = default;
  Return& operator=(const Return&) = default;
  Return& operator=(Return&&) = default;
  virtual ~Return() = default;

  void accept(IVisitor& visitor) override {
    visitor.visitReturnStmt(*this);
  }

  Token m_keyword;
  std::shared_ptr<expr::Expr> m_value;

};

class Var : public Stmt {
public:
  Var(const Token& name, const std::shared_ptr<expr::Expr>& initializer) :
    Stmt(),
    m_name(name),
    m_initializer(initializer)
  {}

  Var(const Var&) = default;
  Var(Var&&) = default;
  Var& operator=(const Var&) = default;
  Var& operator=(Var&&) = default;
  virtual ~Var() = default;

  void accept(IVisitor& visitor) override {
    visitor.visitVarStmt(*this);
  }

  Token m_name;
  std::shared_ptr<expr::Expr> m_initializer;

};

class While : public Stmt {
public:
  While(const std::shared_ptr<expr::Expr>& condition, const std::shared_ptr<Stmt>& then_branch, const std::shared_ptr<Stmt>& else_branch) :
    Stmt(),
    m_condition(condition),
    m_then_branch(then_branch),
    m_else_branch(else_branch)
  {}

  While(const While&) = default;
  While(While&&) = default;
  While& operator=(const While&) = default;
  While& operator=(While&&) = default;
  virtual ~While() = default;

  void accept(IVisitor& visitor) override {
    visitor.visitWhileStmt(*this);
  }

  std::shared_ptr<expr::Expr> m_condition;
  std::shared_ptr<Stmt> m_then_branch;
  std::shared_ptr<Stmt> m_else_branch;

};

} // namespace expr

} // namespace slang

#endif // __SLANG_STMT_HPP__
