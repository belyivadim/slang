#ifndef __SLANG_INSTANCE_HPP__
#define __SLANG_INSTANCE_HPP__

#include <unordered_map>

#include "SlangClass.hpp"

namespace slang {

class SlangInstance {
public:
  SlangInstance(const SlangClass* cls)
    : m_cls(cls) {}

  SlangInstance(SlangInstance &&) = default;
  SlangInstance(const SlangInstance &) = default;
  SlangInstance &operator=(SlangInstance &&) = default;
  SlangInstance &operator=(const SlangInstance &) = default;
  ~SlangInstance() = default;

  string to_string() const;

  Object get_property(const Token& name);
  void set_property(const Token& name, const Object& value);

private:
  std::unordered_map<string, Object> m_fields{};
  const SlangClass* m_cls;
};
  
} // namespace slang

#endif // !__SLANG_INSTANCE_HPP__
