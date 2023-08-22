#ifndef __SLANG_ERROR_REPORTER_HPP__
#define __SLANG_ERROR_REPORTER_HPP__

#include <iostream>
#include <sstream>
#include <string>

#include "Token.hpp"

namespace slang {

class ErrorReporter {
public:
  ErrorReporter() = default;
  ErrorReporter(ErrorReporter &&) = default;
  ErrorReporter(const ErrorReporter &) = default;
  ErrorReporter &operator=(ErrorReporter &&) = default;
  ErrorReporter &operator=(const ErrorReporter &) = default;
  ~ErrorReporter() = default;

  void report(int line, const std::string& where, const std::string& msg) {
    std::stringstream ss;
    ss << "[line " << line << "] Error " << where << ": " << msg;
    std::cout << ss.str() << std::endl;
    m_has_error = true;
  }

  void error(const Token& token, const std::string& msg) {
    if (token.m_type == END_OF_FILE) {
      report(token.m_line, "at end", msg);
    } else {
      report(token.m_line, "at '" + token.m_lexeme + "'", msg);
    }
  }

  void error(int line, const std::string& msg) {
    report(line, "", msg);
  }

  bool has_error() const { return m_has_error; }

  void discard_error_state() { m_has_error = false; }

private:
  bool m_has_error{false};
};
  
} // namespace slang

#endif // __SLANG_ERROR_REPORTER_HPP__
