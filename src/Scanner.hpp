#ifndef __SLANG_SCANNER_HPP__
#define __SLANG_SCANNER_HPP__

#include <memory>
#include <vector>
#include <unordered_map>

#include "Token.hpp"
#include "ErrorReporter.hpp"

namespace slang {

class Scanner {
public:
  Scanner(const std::string& src, std::shared_ptr<ErrorReporter> reporter);

  Scanner(Scanner &&) = default;
  Scanner(const Scanner &) = default;
  Scanner &operator=(Scanner &&) = delete;
  Scanner &operator=(const Scanner &) = delete;
  ~Scanner() = default;

  const std::vector<Token>& scan_tokens();

private:
  const std::string& m_src;
  std::shared_ptr<ErrorReporter> m_reporter;
  std::vector<Token> m_tokens{};
  std::size_t m_start = 0;
  std::size_t m_current = 0;
  std::size_t m_line = 1;

  static std::unordered_map<std::string, TokenType> s_keywords;

  bool is_at_end() const;
  void scan_token();
  bool match(char expected);
  char peek() const;
  char peek_next() const;
  void skip_comment();
  void process_string();
  void process_number();
  void process_identifier();
  char advance();
  void add_token(TokenType type, Token::Literal literal = nullptr);

};

} // namespace slang

#endif // __SLANG_SCANNER_HPP__
