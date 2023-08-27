#ifndef __SLANG_STMT_HPP__
#define __SLANG_STMT_HPP__

#include <memory>
#include <vector>

#include "Token.hpp"
#include "Expr.hpp"

namespace slang {

namespace stmt {

class Block;
class Expression;
class Print;
class Var;

class IVisitor {
public:
  IVisitor() = default;
  IVisitor(const IVisitor&) = default;
  IVisitor(IVisitor&&) = default;
  IVisitor& operator=(const IVisitor&) = default;
  IVisitor& operator=(IVisitor&&) = default;
  virtual ~IVisitor() = default;

  virtual void visitBlockStmt(Block& stmt) = 0;
  virtual void visitExpressionStmt(Expression& stmt) = 0;
  virtual void visitPrintStmt(Print& stmt) = 0;
  virtual void visitVarStmt(Var& stmt) = 0;
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

} // namespace expr

} // namespace slang

#endif // __SLANG_STMT_HPP__
