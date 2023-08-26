#ifndef __SLANG_OBJECT_HPP__
#define __SLANG_OBJECT_HPP__

#include <string>
#include <variant>

namespace slang {

  using Object = std::variant<double, bool, std::string, std::nullptr_t>;

inline std::string object_to_string(const Object& obj) {
  if (const double * pval = std::get_if<double>(&obj)) {
    return std::to_string(*pval);
  } else if (const bool * pval = std::get_if<bool>(&obj)) {
    return *pval ? "true" : "false";
  } else if (const std::string * pval = std::get_if<std::string>(&obj)) {
    return *pval;
  } else if (std::holds_alternative<std::nullptr_t>(obj)) {
    return "none";
  }

  return "";
}

} // namespace slang

#endif // __SLANG_OBJECT_HPP__
