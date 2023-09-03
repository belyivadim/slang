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
    statments.push_back(declaration());
  }

  return statments;
}


// ------------------------ | PRIVATE |
//
// ------------------------ | RULES |
shared_ptr<stmt::Stmt> Parser::declaration() {
  try {
    if (match({LET})) return var_declaration();

    return statement();
  } catch (const ParserError& e) {
    sync();
    return nullptr;
  }
}

shared_ptr<stmt::Stmt> Parser::var_declaration() {
  auto& name = consume(IDENTIFIER, "Expect variable name.");

  shared_ptr<expr::Expr> initializer;
  if (match({EQ})) {
    initializer = expression();
  }

  consume(SEMICOLON, "Expect ';' after variable declaration.");
  return make_shared<stmt::Var>(stmt::Var(name, initializer));
}

shared_ptr<stmt::Fn> Parser::function(const string& kind) {
  auto& name = consume(IDENTIFIER, "Expect " + kind + " name.");
  consume(LEFT_PAREN, "Expect '(' after " + kind + " name.");

  vector<Token> params;
  if (!check(RIGHT_PAREN)) {
    do {
      if (params.size() >= 255) {
        m_reporter->error(peek(), "Cannot have more than 255 parameters.");
      }

      params.push_back(consume(IDENTIFIER, "Expect parameter name."));
    } while(match({COMMA}));
  }

  consume(RIGHT_PAREN, "Expect ')' after parameters.");

  vector<shared_ptr<stmt::Stmt>> body;

  if (match({LEFT_BRACE})) {
    body = block();
  } else if (match({EQ_GREATER})) {
    // arrow function
    auto& keyword = previous();
    auto value = expression();
    auto stmt_ret = make_shared<stmt::Return>(stmt::Return(keyword, value));
    body.push_back(stmt_ret);
    consume(SEMICOLON, "Expect ';' after arrow " + kind + " body.");
  } else {
    consume(LEFT_BRACE, "Expect '{' before " + kind + " body.");
  }

  return make_shared<stmt::Fn>(stmt::Fn(name, params, body));
}

shared_ptr<stmt::Stmt> Parser::statement() {
  if (match({FOR})) return for_statement();
  if (match({IF})) return if_statement();
  if (match({FN})) return function("function");
  if (match({PRINT})) return print_statement();
  if (match({RETURN})) return return_statement();
  if (match({WHILE})) return while_statement();
  if (match({LEFT_BRACE})) 
    return make_shared<stmt::Block>(stmt::Block(block()));

  return expression_statement();
}


shared_ptr<stmt::Stmt> Parser::return_statement() {
  auto& keyword = previous();
  std::shared_ptr<expr::Expr> value = nullptr;

  if (!check(SEMICOLON)) {
    value = expression();
  }

  consume(SEMICOLON, "Expect ';' after return value.");
  return make_shared<stmt::Return>(stmt::Return(keyword, value));
}

shared_ptr<stmt::Stmt> Parser::if_statement() {
  consume(LEFT_PAREN, "Expected '(' after 'if'.");
  auto condition = expression();
  consume(RIGHT_PAREN, "Expected ')' after if condition.");

  auto then_branch = statement();
  shared_ptr<stmt::Stmt> else_branch = match({ELSE}) ? statement() : nullptr;

  return make_shared<stmt::If>(stmt::If(condition, then_branch, else_branch));
}

shared_ptr<stmt::Stmt> Parser::while_statement() {
  consume(LEFT_PAREN, "Expected '(' after 'while'.");
  auto condition = expression();
  consume(RIGHT_PAREN, "Expected ')' after while condition.");

  auto then_branch = statement();
  shared_ptr<stmt::Stmt> else_branch = match({ELSE}) ? statement() : nullptr;

  return make_shared<stmt::While>(stmt::While(condition, then_branch, else_branch));
}

shared_ptr<stmt::Stmt> Parser::for_statement() {
  consume(LEFT_PAREN, "Expected '(' after 'for'.");

  shared_ptr<stmt::Stmt> initializer{nullptr};
  if (match({LET})) {
    initializer = var_declaration();
  } else if (!match({SEMICOLON})) {
    initializer = expression_statement();
  }

  shared_ptr<expr::Expr> condition{nullptr};
  if (!check(SEMICOLON)) {
    condition = expression();
  }
  consume(SEMICOLON, "Expect ';' after loop condition.");

  shared_ptr<expr::Expr> increment{nullptr};
  if (!check(RIGHT_PAREN)) {
    increment = expression();
  }
  consume(RIGHT_PAREN, "Expect ')' after for clauses.");

  shared_ptr<stmt::Stmt> body = statement();

  if (increment != nullptr) {
    body = make_shared<stmt::Block>(
      stmt::Block({body, make_shared<stmt::Expression>(increment)})
    );
  }

  if (condition == nullptr) {
    condition = make_shared<expr::Literal>(expr::Literal(true));
  }

  body = make_shared<stmt::While>(
    stmt::While(condition, body, nullptr)
  );

  if (initializer != nullptr) {
    body = make_shared<stmt::Block>(
      stmt::Block({initializer, body})
    );
  }

  return body;
}

vector<shared_ptr<stmt::Stmt>> Parser::block() {
  vector<shared_ptr<stmt::Stmt>> statements;

  while (!check(RIGHT_BRACE) && !is_at_end()) {
    statements.push_back(declaration());
  }

  consume(RIGHT_BRACE, "Expect '}' after block.");
  return statements;
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
  return assigment();
}


shared_ptr<expr::Expr> Parser::assigment() {
  auto expr = or_();

  if (match({EQ})) {
    auto& equals = previous();
    auto value = assigment();

    if (expr::Variable *v = dynamic_cast<expr::Variable*>(expr.get())) {
      Token name = v->m_name;
      return make_shared<expr::Assign>(name, value);
    }

    error(equals, "Invalid assigment target.");
  }

  return expr;
}

shared_ptr<expr::Expr> Parser::or_() {
  auto expr = and_();

  while (match({OR})) {
    auto& oper = previous();
    auto right = and_();
    expr = make_shared<expr::Logical>(expr::Logical(expr, oper, right));
  }

  return expr;
}

shared_ptr<expr::Expr> Parser::and_() {
  auto expr = equality();

  while (match({AND})) {
    auto& oper = previous();
    auto right = equality();
    expr = make_shared<expr::Logical>(expr::Logical(expr, oper, right));
  }

  return expr;
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

  return call();
}


shared_ptr<expr::Expr> Parser::call() {
  auto expr = primary();

  while (true) {
    if (match({LEFT_PAREN})) {
      expr = finish_call(expr);
    } else {
      break;
    }
  }

  return expr;
}

shared_ptr<expr::Expr> Parser::finish_call(shared_ptr<expr::Expr>& callee) {
  vector<shared_ptr<expr::Expr>> args;

  if (!check(RIGHT_PAREN)) {
    do {
      if (args.size() >= 255) {
        m_reporter->error(peek(), "Can't have more than 255 arguments.");
      }
      args.push_back(expression());
    } while (match({COMMA}));
  }

  auto& paren = consume(RIGHT_PAREN, "Exprect ')' after arguments.");

  return make_shared<expr::Call>(expr::Call(callee, paren, args));
}

shared_ptr<expr::Expr> Parser::primary() {
  if (match({FALSE})) return make_shared<expr::Literal>(expr::Literal(false));
  if (match({TRUE})) return make_shared<expr::Literal>(expr::Literal(true));
  if (match({NONE})) return make_shared<expr::Literal>(expr::Literal(nullptr));

  if (match({NUMBER, STRING})) {
    return make_shared<expr::Literal>(expr::Literal(previous().m_literal));
  }

  if (match({IDENTIFIER})) return make_shared<expr::Variable>(previous());

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
