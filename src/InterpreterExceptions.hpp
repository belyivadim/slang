#ifndef __SLANG_INTERPRETER_EXCEPTIONS_HPP__
#define __SLANG_INTERPRETER_EXCEPTIONS_HPP__

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

class ReturnExc : public std::runtime_error {
public:
  ReturnExc(const Object& value) 
    : std::runtime_error(""), m_value(value) {}

  ReturnExc(ReturnExc &&) = default;
  ReturnExc(const ReturnExc &) = default;
  ReturnExc &operator=(ReturnExc &&) = default;
  ReturnExc &operator=(const ReturnExc &) = default;
  ~ReturnExc() = default;

  Object m_value;
  
};

class BreakExc : public std::runtime_error {
public:
  BreakExc() : std::runtime_error("") {}

  BreakExc(BreakExc &&) = default;
  BreakExc(const BreakExc &) = default;
  BreakExc &operator=(BreakExc &&) = default;
  BreakExc &operator=(const BreakExc &) = default;
  ~BreakExc() = default;
};


} // namespace slang

#endif // __SLANG_INTERPRETER_EXCEPTIONS_HPP__
