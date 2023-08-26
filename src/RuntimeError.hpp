#ifndef __SLANG_RUNTIME_ERROR_HPP__
#define __SLANG_RUNTIME_ERROR_HPP__

#include <stdexcept>

#include "Token.hpp"

namespace slang {

class RuntimeError : public std::runtime_error {
public:
  RuntimeError(const Token& token, const std::string& msg) :
    std::runtime_error(msg), m_token(token) {}

  RuntimeError(RuntimeError &&) = default;
  RuntimeError(const RuntimeError &) = default;
  RuntimeError &operator=(RuntimeError &&) = delete;
  RuntimeError &operator=(const RuntimeError &) = delete;
  ~RuntimeError() = default;


  const Token& m_token;

};

} // namespace slang

#endif // __SLANG_RUNTIME_ERROR_HPP__
