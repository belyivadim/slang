#include "Slang.hpp"
#include "Token.hpp"

int main (int argc, char *argv[]) {
  slang::Slang slang;

  if (argc > 2) {
    return 64;
  } else if (argc == 2) {
    return slang.run_file(argv[1]);
  } else {
    return slang.run_promt();
  }
}
