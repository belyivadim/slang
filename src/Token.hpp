#ifndef __SLANG_TOKEN_HPP__
#define __SLANG_TOKEN_HPP__

#include <string>
#include <variant>

namespace slang {

enum TokenType {
  // single-character tokens
  LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
  COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

  // one or two characer tokens
  BANG, BANG_EQ, EQ, EQ_EQ,
  GREATER, GREATER_EQ, LESS, LESS_EQ,

  // literals
  IDENTIFIER, STRING, NUMBER,

  // keywords
  AND, BASE, CLASS, ELSE, FALSE, FOR, FN, IF, LET,
  NONE, OR, PRINT, RETURN, SELF, TRUE, WHILE,

  END_OF_FILE
};


class Token {
public:

  using Literal = std::variant<double, bool, std::string, void *>;

  Token(TokenType type, const std::string& lexeme, Literal literal, int line)
    : m_type(type), m_lexeme(lexeme), m_literal(literal), m_line(line) {}

  Token(Token &&) = default;
  Token(const Token &) = default;
  Token &operator=(Token &&) = delete; // deleted because of const fields
  Token &operator=(const Token &) = delete;
  ~Token() = default;

  std::string to_string() const {
    return std::to_string(m_type) + " " + m_lexeme + " " + literal_to_string(m_literal);
  }

  static std::string literal_to_string(const Token::Literal& literal) {
    if (const double * pval = std::get_if<double>(&literal)) {
      return std::to_string(*pval);
    } else if (const bool * pval = std::get_if<bool>(&literal)) {
      return *pval ? "true" : "false";
    } else if (const std::string * pval = std::get_if<std::string>(&literal)) {
      return *pval;
    }

    return "";
  }

  const TokenType m_type;
  const std::string m_lexeme;
  const Literal m_literal;
  const std::size_t m_line;
  
};



} // namespace slang

#endif // __SLANG_TOKEN_HPP__
