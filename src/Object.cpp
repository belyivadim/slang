#include <iostream>

#include "Object.hpp"
#include "ICallable.hpp"

namespace slang {

std::string object_to_string(const Object& obj) {
  if (const double * pval = std::get_if<double>(&obj)) {
    constexpr char trailing_zeros[] = ".000000";
    constexpr size_t tz_size = sizeof(trailing_zeros) - 1;
    auto str = std::to_string(*pval);

    if (0 == str.compare(str.length() - tz_size, tz_size, trailing_zeros)) {
      str = str.substr(0, str.length() - tz_size);
    }

    return str;
  } else if (const std::shared_ptr<ICallable> * pval = std::get_if<std::shared_ptr<ICallable>>(&obj)) {
    return pval->get()->to_string();
  } else if (const bool * pval = std::get_if<bool>(&obj)) {
    return *pval ? "true" : "false";
  } else if (const std::string * pval = std::get_if<std::string>(&obj)) {
    return *pval;
  } else if (std::holds_alternative<std::nullptr_t>(obj)) {
    return "none";
  }

  return "";
}

}
