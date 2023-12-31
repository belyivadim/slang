#ifndef __SLANG_PARSER_HPP__
#define __SLANG_PARSER_HPP__

#include <stdexcept>
#include <vector>

#include "Token.hpp"
#include "Expr.hpp"
#include "Stmt.hpp"
#include "ErrorReporter.hpp"

namespace slang {

using std::vector;
using std::shared_ptr;
using std::string;

class Parser {
public:
  Parser(const vector<Token>& tokens, shared_ptr<ErrorReporter> reporter);
  Parser(Parser &&) = default;
  Parser(const Parser &) = default;
  Parser &operator=(Parser &&) = delete; // delete because of const member
  Parser &operator=(const Parser &) = delete;
  ~Parser() = default;

  vector<shared_ptr<stmt::Stmt>> parse();

private:
  const vector<Token>& m_tokens;
  std::size_t m_current = 0;
  shared_ptr<ErrorReporter> m_reporter;

  class ParserError : public std::runtime_error {
  public:
    ParserError()
      : std::runtime_error("ParserError")
    {}
  };

  shared_ptr<stmt::Stmt> declaration();
  shared_ptr<stmt::Stmt> var_declaration();
  shared_ptr<stmt::Stmt> class_declaration();
  shared_ptr<stmt::Fn> function(const string& kind);
  shared_ptr<stmt::Stmt> statement();
  shared_ptr<stmt::Stmt> break_statement();
  shared_ptr<stmt::Stmt> return_statement();
  shared_ptr<stmt::Stmt> if_statement();
  shared_ptr<stmt::Stmt> while_statement();
  shared_ptr<stmt::Stmt> for_statement();
  vector<shared_ptr<stmt::Stmt>> block();
  shared_ptr<stmt::Stmt> print_statement();
  shared_ptr<stmt::Stmt> expression_statement();

  shared_ptr<expr::Expr> expression();
  shared_ptr<expr::Expr> assigment();
  shared_ptr<expr::Expr> or_();
  shared_ptr<expr::Expr> and_();
  shared_ptr<expr::Expr> equality();
  shared_ptr<expr::Expr> comprasion();
  shared_ptr<expr::Expr> term();
  shared_ptr<expr::Expr> factor();
  shared_ptr<expr::Expr> unary();
  shared_ptr<expr::Expr> call();
  shared_ptr<expr::Expr> primary();

  bool match(const vector<TokenType>& types);
  bool check(TokenType type) const;
  bool is_at_end() const;
  const Token& previous() const;
  const Token& peek() const;
  const Token& advance();

  const Token& consume(TokenType type, const string& msg);
  ParserError error(const Token& token, const string& msg);

  shared_ptr<expr::Expr> finish_call(shared_ptr<expr::Expr>& callee);

  void sync();

};

} // namespace slang

#endif // !__SLANG_PARSER_HPP__
