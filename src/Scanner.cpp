#include "Scanner.hpp"
#include <cctype>
#include <cstdlib>

namespace slang {

// ------------------------ | INIT STATICS |
std::unordered_map<std::string, TokenType> Scanner::s_keywords{
  { "and",      AND },
  { "base",     BASE },
  { "break",    BREAK },
  { "class",    CLASS },
  { "else",     ELSE },
  { "false",    FALSE },
  { "fn",       FN },
  { "for",      FOR },
  { "if",       IF },
  { "let",      LET },
  { "none",     NONE },
  { "or",       OR },
  { "print",    PRINT },
  { "return",   RETURN },
  { "self",     SELF },
  { "true",     TRUE },
  { "while",    WHILE }
};

// ------------------------ | HELPERS |
namespace helpers {

static bool is_alpha(char c) {
  return (c >= 'a' && c <= 'z') ||
         (c >= 'A' && c <= 'Z') ||
         c == '_';
}

static bool is_alpha_or_digit(char c) {
  return is_alpha(c) || isdigit(c);
}

} // namespace helpers


// ------------------------ | PUBLIC |
Scanner::Scanner(const std::string& src, std::shared_ptr<ErrorReporter> reporter)
    : m_src(src), m_reporter(reporter) 
{
}


const std::vector<Token>& Scanner::scan_tokens() {
  while (!is_at_end()) {
    // new lexeme
    m_start = m_current;
    scan_token();
  }

  m_tokens.push_back(Token(END_OF_FILE, "", nullptr, m_line));
  return m_tokens;
}


// ------------------------ | PRIVATE |
bool Scanner::is_at_end() const { return m_current >= m_src.size(); }

void Scanner::scan_token() {
  char c = advance();

  switch (c) {
    case '(': add_token(LEFT_PAREN); break;
    case ')': add_token(RIGHT_PAREN); break;
    case '{': add_token(LEFT_BRACE); break;
    case '}': add_token(RIGHT_BRACE); break;
    case ',': add_token(COMMA); break;
    case '.': add_token(DOT); break;
    case '-': add_token(MINUS); break;
    case '+': add_token(PLUS); break;
    case ';': add_token(SEMICOLON); break;
    case '*': add_token(STAR); break;

    case '!':
      add_token(match('=') ? BANG_EQ : BANG);
      break;
   case '=':
      if (match('=')) {
        add_token(EQ_EQ);
      } else if (match('>')) {
        add_token(EQ_GREATER);
      } else {
        add_token(EQ);
      }
      break;
    case '<':
      add_token(match('=') ? LESS_EQ : LESS);
      break;
    case '>':
      add_token(match('=') ? GREATER_EQ : GREATER);
      break;

    case '/':
      if (match('/')) {
        skip_comment();
      } else {
        add_token(SLASH);
      }
      break;

    case ' ':
    case '\r':
    case '\t':
      // ignore whitespace
      break;

    case '\n':
      ++m_line;
      break;

    case '"': process_string(); break;

    default: 
      if (isdigit(c)) {
        process_number();
      } else if (helpers::is_alpha(c)) {
        process_identifier();
      } else {
        m_reporter->error(m_line, "Unexpected character.");
      }
      break;
  }
}


bool Scanner::match(char expected) {
  if (is_at_end() || m_src[m_current] != expected) return false;

  ++m_current;
  return true;
}

char Scanner::peek() const {
  if (is_at_end()) return '\0';

  return m_src[m_current];
}

void Scanner::skip_comment() {
  while(peek() != '\n' && !is_at_end()) {
    advance();
  }
}


void Scanner::process_string() {
  char c{};
  while ((c = peek()) != '"' && !is_at_end()) {
    m_line += '\n' == c;
    advance();
  }

  if (is_at_end()) {
    m_reporter->error(m_line, "Unterminated string.");
    return;
  }

  advance(); // closing "

  // trim the surrounding quotes
  const std::string& literal = m_src.substr(m_start + 1, m_current - m_start - 2);
  add_token(STRING, literal);
}

void Scanner::process_number() {
  while (isdigit(peek())) {
    advance();
  }

  // fractional part
  if (peek() == '.' && isdigit(peek_next())) {
    advance(); // .

    while (isdigit(peek())) {
      advance();
    }
  }

  double literal = std::strtod(m_src.substr(m_start, m_current - m_start).c_str(), NULL);
  add_token(NUMBER, literal);
}

void Scanner::process_identifier() {
  while (helpers::is_alpha_or_digit(peek())) {
    advance();
  }

  const std::string& text = m_src.substr(m_start, m_current - m_start);
  auto it = s_keywords.find(text);
  TokenType type = it == s_keywords.end() ? IDENTIFIER : it->second;
  add_token(type);
}

char Scanner::peek_next() const {
  if (m_current + 1 >= m_src.size()) return '\0';

  return m_src[m_current + 1];
}

char Scanner::advance() { return m_src[m_current++]; }

void Scanner::add_token(TokenType type, Object literal) {
  std::string lexeme = m_src.substr(m_start, m_current - m_start);
  m_tokens.push_back(Token(type, lexeme, literal, m_line));
}



} // namespace slang
