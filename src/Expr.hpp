#ifndef __SLANG_EXPR_HPP__
#define __SLANG_EXPR_HPP__

#include <memory>

#include "Token.hpp"

namespace slang {

namespace expr {

class Assign;
class Binary;
class Grouping;
class Literal;
class Logical;
class Unary;
class Variable;

class IVisitor {
public:
  IVisitor() = default;
  IVisitor(const IVisitor&) = default;
  IVisitor(IVisitor&&) = default;
  IVisitor& operator=(const IVisitor&) = default;
  IVisitor& operator=(IVisitor&&) = default;
  virtual ~IVisitor() = default;

  virtual void visitAssignExpr(Assign& expr) = 0;
  virtual void visitBinaryExpr(Binary& expr) = 0;
  virtual void visitGroupingExpr(Grouping& expr) = 0;
  virtual void visitLiteralExpr(Literal& expr) = 0;
  virtual void visitLogicalExpr(Logical& expr) = 0;
  virtual void visitUnaryExpr(Unary& expr) = 0;
  virtual void visitVariableExpr(Variable& expr) = 0;
};

class Expr {
public:
  Expr() = default;
  Expr(const Expr&) = default;
  Expr(Expr&&) = default;
  Expr& operator=(const Expr&) = default;
  Expr& operator=(Expr&&) = default;
  virtual ~Expr() = default;

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

class Assign : public Expr {
public:
  Assign(const Token& name, const std::shared_ptr<Expr>& value) :
    Expr(),
    m_name(name),
    m_value(value)
  {}

  Assign(const Assign&) = default;
  Assign(Assign&&) = default;
  Assign& operator=(const Assign&) = default;
  Assign& operator=(Assign&&) = default;
  virtual ~Assign() = default;

  void accept(IVisitor& visitor) override {
    visitor.visitAssignExpr(*this);
  }

  Token m_name;
  std::shared_ptr<Expr> m_value;

};

class Binary : public Expr {
public:
  Binary(const std::shared_ptr<Expr>& left, const Token& oper, const std::shared_ptr<Expr>& right) :
    Expr(),
    m_left(left),
    m_oper(oper),
    m_right(right)
  {}

  Binary(const Binary&) = default;
  Binary(Binary&&) = default;
  Binary& operator=(const Binary&) = default;
  Binary& operator=(Binary&&) = default;
  virtual ~Binary() = default;

  void accept(IVisitor& visitor) override {
    visitor.visitBinaryExpr(*this);
  }

  std::shared_ptr<Expr> m_left;
  Token m_oper;
  std::shared_ptr<Expr> m_right;

};

class Grouping : public Expr {
public:
  Grouping(const std::shared_ptr<Expr>& expression) :
    Expr(),
    m_expression(expression)
  {}

  Grouping(const Grouping&) = default;
  Grouping(Grouping&&) = default;
  Grouping& operator=(const Grouping&) = default;
  Grouping& operator=(Grouping&&) = default;
  virtual ~Grouping() = default;

  void accept(IVisitor& visitor) override {
    visitor.visitGroupingExpr(*this);
  }

  std::shared_ptr<Expr> m_expression;

};

class Literal : public Expr {
public:
  Literal(const Object& value) :
    Expr(),
    m_value(value)
  {}

  Literal(const Literal&) = default;
  Literal(Literal&&) = default;
  Literal& operator=(const Literal&) = default;
  Literal& operator=(Literal&&) = default;
  virtual ~Literal() = default;

  void accept(IVisitor& visitor) override {
    visitor.visitLiteralExpr(*this);
  }

  Object m_value;

};

class Logical : public Expr {
public:
  Logical(const std::shared_ptr<Expr>& left, const Token& oper, const std::shared_ptr<Expr>& right) :
    Expr(),
    m_left(left),
    m_oper(oper),
    m_right(right)
  {}

  Logical(const Logical&) = default;
  Logical(Logical&&) = default;
  Logical& operator=(const Logical&) = default;
  Logical& operator=(Logical&&) = default;
  virtual ~Logical() = default;

  void accept(IVisitor& visitor) override {
    visitor.visitLogicalExpr(*this);
  }

  std::shared_ptr<Expr> m_left;
  Token m_oper;
  std::shared_ptr<Expr> m_right;

};

class Unary : public Expr {
public:
  Unary(const Token& oper, const std::shared_ptr<Expr>& right) :
    Expr(),
    m_oper(oper),
    m_right(right)
  {}

  Unary(const Unary&) = default;
  Unary(Unary&&) = default;
  Unary& operator=(const Unary&) = default;
  Unary& operator=(Unary&&) = default;
  virtual ~Unary() = default;

  void accept(IVisitor& visitor) override {
    visitor.visitUnaryExpr(*this);
  }

  Token m_oper;
  std::shared_ptr<Expr> m_right;

};

class Variable : public Expr {
public:
  Variable(const Token& name) :
    Expr(),
    m_name(name)
  {}

  Variable(const Variable&) = default;
  Variable(Variable&&) = default;
  Variable& operator=(const Variable&) = default;
  Variable& operator=(Variable&&) = default;
  virtual ~Variable() = default;

  void accept(IVisitor& visitor) override {
    visitor.visitVariableExpr(*this);
  }

  Token m_name;

};

} // namespace expr

} // namespace slang

#endif // __SLANG_EXPR_HPP__
