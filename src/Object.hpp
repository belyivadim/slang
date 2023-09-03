#ifndef __SLANG_OBJECT_HPP__
#define __SLANG_OBJECT_HPP__

#include <string>
#include <variant>
#include <memory>

namespace slang {

class ICallable;
class SlangClass;
class SlangInstance;

using Object = std::variant<double, bool, std::string, 
                            std::shared_ptr<ICallable>, ICallable*,
                            std::shared_ptr<SlangInstance>,
                            std::nullptr_t>;

std::string object_to_string(const Object& obj);

} // namespace slang

#endif // __SLANG_OBJECT_HPP__
