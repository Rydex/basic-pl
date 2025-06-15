#include <iostream>
#include "src/lexer.h"

int main() {
  std::string input;
  do {
    std::cout << "program (type exit to exit) > ";
    std::getline(std::cin, input);

    const auto&[ast, error] = run("<stdin>", input);

    // if(error) {
    //   std::cout << error->as_string() << '\n';
    // } else {
    //   std::cout << '[';

    //   for(size_t i=0; i<tokens.size(); ++i) {
    //     std::cout << tokens[i].as_string();

    //     if(i != tokens.size()-1) {
    //       std::cout << ", ";
    //     }
    //   }

    //   std::cout << "]\n";
    // }

    if(error) { // if theres an error print it as string
      std::cout << error->as_string() << '\n'; 
    } else {
      if(ast) { // if theres ast print the value as string
        // if(std::holds_alternative<NumberNode>(ast.value())) {
        //   std::cout << std::get<NumberNode>(ast.value()).as_string() << '\n';
        // } else if(std::holds_alternative<SharedUnary>(ast.value())) {
        //   std::cout << std::get<SharedUnary>(ast.value())->as_string() << '\n';
        // } else {
        //   std::cout << std::get<SharedBin>(ast.value())->as_string() << '\n';
        // }
        std::cout << ast->as_string() << '\n';
      }
    }
  } while (input != "exit");
}