#include <iostream>
#include "src/lexer.h"

int main() {
  std::string input;
  do {
    std::cout << "program (type q to quit) > ";
    std::getline(std::cin, input);

    const auto&[ast, error] = run("<stdin>", input);

    if(error) { // if theres an error print it as string
      std::cout << error->as_string() << '\n'; 
    } else {
      if(ast) {
        std::cout << ast->as_string() << '\n';
      }
    }
  } while (input != "q");
}