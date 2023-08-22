#ifndef __SLANG_SLANG_HPP__
#define __SLANG_SLANG_HPP__

#include <fstream>
#include <memory>
#include <vector>

#include "Scanner.hpp"
#include "Parser.hpp"
#include "AstPrinter.hpp"

namespace slang {

class Slang {
public:
  Slang() = default;
  Slang(Slang &&) = default;
  Slang(const Slang &) = default;
  Slang &operator=(Slang &&) = default;
  Slang &operator=(const Slang &) = default;
  ~Slang() = default;

  int run_file(const char *path) {
    if (-1 == read_file(path)) {
      return 1;
    }

    return run(m_src);
  }

  int run_promt() {
    for (;;) {
      std::cout << "> ";
      std::string line;
      std::getline(std::cin, line);

      if (0 == line.size()) break;

      run(line);
      m_reporter->discard_error_state();
    }

    return 0;
  }

  
private:
  std::string m_src;
  std::shared_ptr<ErrorReporter> m_reporter{new ErrorReporter};

  int run(const std::string& src) {
    Scanner scanner(src, m_reporter);
    auto tokens = scanner.scan_tokens();

    Parser parser(tokens, m_reporter);
    auto expr = parser.parse();

    if (m_reporter->has_error()) {
      return 65;
    }


    AstPrinter printer;
    std::cout << printer.print(*expr) << std::endl;

    return 0;
  }

  int read_file(const char* path) {
    std::ifstream file(path);
    if (!file) {
      return 1;
    }

    m_src = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return 0;
  }

};

} // namespace slang

#endif // __SLANG_SLANG_HPP__
