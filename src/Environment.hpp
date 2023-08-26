#ifndef __SLANG_ENVIRONMENT_HPP__
#define __SLANG_ENVIRONMENT_HPP__

#include <unordered_map>
#include <string>

#include "RuntimeError.hpp"
#include "Token.hpp"

namespace slang {

class Environment {
public:
  Environment() = default;
  Environment(Environment &&) = default;
  Environment(const Environment &) = default;
  Environment &operator=(Environment &&) = default;
  Environment &operator=(const Environment &) = default;
  ~Environment() = default;

  void define(const std::string& name, const Object& value) {
    variables[name] = value;
  }

  void assign(const Token& name, const Object& value) {
    auto found = get_iter(name);
    found->second = value;
  }

  Object& get(const Token& name) {
    auto found = get_iter(name);
    return found->second;
  }

private:
  std::unordered_map<std::string, Object> variables{};


  using iterator = std::unordered_map<std::string, Object>::iterator;

  iterator get_iter(const Token& name) {
    auto found = variables.find(name.m_lexeme);

    if (found == variables.end()) {
      throw RuntimeError(name, "Undefined variable '" + name.m_lexeme + "'.");
    }

    return found;
  }

};

} // namespace slang

#endif // __SLANG_ENVIRONMENT_HPP__
