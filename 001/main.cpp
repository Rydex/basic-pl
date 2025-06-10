#include <iostream>
#include "src/lexer.h"

int main() {
  std::string input;
  do {
    std::cout << "program (type exit to exit) > ";
    std::getline(std::cin, input);

    const auto&[tokens, error] = run("<stdin>", input);

    if(error) {
      std::cout << error->as_string() << '\n';
    } else {
      std::cout << '[';

      for(size_t i=0; i<tokens.size(); ++i) {
        std::cout << tokens[i].as_string();

        if(i != tokens.size()-1) {
          std::cout << ", ";
        }
      }

      std::cout << "]\n";
    }
  } while (input != "exit");
}