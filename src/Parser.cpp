#include "Parser.hpp"

namespace slang {

using std::make_shared;

// ------------------------ | PUBLIC |
Parser::Parser(const vector<Token>& tokens, shared_ptr<ErrorReporter> reporter)
  : m_tokens(tokens),
    m_reporter(reporter)
{}


vector<shared_ptr<stmt::Stmt>> Parser::parse() {
  vector<shared_ptr<stmt::Stmt>> statments;

  while (!is_at_end()) {
    statments.push_back(statement());
  }

  return statments;
}


// ------------------------ | PRIVATE |
//
// ------------------------ | RULES |
shared_ptr<stmt::Stmt> Parser::statement() {
  if (match({PRINT})) return print_statement();

  return expression_statement();
}

shared_ptr<stmt::Stmt> Parser::print_statement() {
  auto value = expression();
  consume(SEMICOLON, "Exprect ';' after value.");
  return make_shared<stmt::Print>(stmt::Print(value));
}

shared_ptr<stmt::Stmt> Parser::expression_statement() {
  auto expr = expression();
  consume(SEMICOLON, "Exprect ';' after expression.");
  return make_shared<stmt::Expression>(stmt::Expression(expr));
}

shared_ptr<expr::Expr> Parser::expression() {
  return equality();
}

shared_ptr<expr::Expr> Parser::equality() {
  auto expr = comprasion();

  while (match({BANG_EQ, EQ_EQ})) {
    auto& oper = previous();
    auto right = comprasion();
    expr = make_shared<expr::Binary>(expr::Binary(expr, oper, right));
  }

  return expr;
}

shared_ptr<expr::Expr> Parser::comprasion() {
  auto expr = term();

  while (match({GREATER, GREATER_EQ, LESS, LESS_EQ})) {
    auto& oper = previous();
    auto right = term();
    expr = make_shared<expr::Binary>(expr::Binary(expr, oper, right));
  }

  return expr;
}

shared_ptr<expr::Expr> Parser::term() {
  auto expr = factor();

  while (match({MINUS, PLUS})) {
    auto& oper = previous();
    auto right = factor();
    expr = make_shared<expr::Binary>(expr::Binary(expr, oper, right));
  }

  return expr;
}

shared_ptr<expr::Expr> Parser::factor() {
  auto expr = unary();

  while (match({SLASH, STAR})) {
    auto& oper = previous();
    auto right = unary();
    expr = make_shared<expr::Binary>(expr::Binary(expr, oper, right));
  }

  return expr;
}

shared_ptr<expr::Expr> Parser::unary() {
  if (match({BANG, MINUS})) {
    auto& oper = previous();
    auto right = unary();
    return make_shared<expr::Unary>(expr::Unary(oper, right));
  }

  return primary();
}

shared_ptr<expr::Expr> Parser::primary() {
  if (match({FALSE})) return make_shared<expr::Literal>(expr::Literal(false));
  if (match({TRUE})) return make_shared<expr::Literal>(expr::Literal(true));
  if (match({NONE})) return make_shared<expr::Literal>(expr::Literal(nullptr));

  if (match({NUMBER, STRING})) {
    return make_shared<expr::Literal>(expr::Literal(previous().m_literal));
  }

  if (match({LEFT_PAREN})) {
    auto expr = expression();
    consume(RIGHT_PAREN, "Expect ')' after expression.");
    return make_shared<expr::Grouping>(expr::Grouping(expr));
  }

  throw error(peek(), "Expect expression.");
}

// ------------------------ | HELPERS |
bool Parser::match(const vector<TokenType>& types) {
  for (auto t : types) {
    if (check(t)) {
      advance();
      return true;
    }
  }

  return false;
}

bool Parser::check(TokenType type) const {
  return !is_at_end() && peek().m_type == type;
}

bool Parser::is_at_end() const {
  return peek().m_type == END_OF_FILE;
}

const Token& Parser::peek() const {
  return m_tokens[m_current];
}

const Token& Parser::previous() const {
  return m_tokens[m_current - 1];
}

const Token& Parser::advance() {
  if (!is_at_end()) ++m_current;
  return previous();
}


const Token& Parser::consume(TokenType type, const string& msg) {
  if (check(type)) return advance();

  throw error(peek(), msg);
}

Parser::ParserError Parser::error(const Token& token, const string& msg) {
  m_reporter->error(token,  msg);
  return Parser::ParserError();
}

void Parser::sync() {
  advance();

  while (!is_at_end()) {
    if (previous().m_type == SEMICOLON) return;

    switch (peek().m_type) {
      case CLASS:
      case FN:
      case LET:
      case FOR:
      case IF:
      case WHILE:
      case PRINT:
      case RETURN:
        return;

      default:
        advance();
        break;
    }

  }
}

} // namespace slang
