#ifndef __SLANG_OBJECT_HPP__
#define __SLANG_OBJECT_HPP__

#include <string>
#include <variant>

namespace slang {

  using Object = std::variant<double, bool, std::string, std::nullptr_t>;

} // namespace slang

#endif // __SLANG_OBJECT_HPP__
