#include <iostream>
#include <sstream>
#include "src/lexer.h"

int main() {
  std::string input;
  do {
    std::cout << "program (type quit to quit) > ";
    std::getline(std::cin, input);

    const auto&[ast, error] = run("<stdin>", input);

    if(error) { // if theres an error print it as string
      std::cout << error->as_string() << '\n'; 
    } else {

      std::visit([&](const auto& val) -> void {
        using T = std::decay_t<decltype(val)>;
        
        if constexpr (std::is_same_v<T, std::string>) {
          std::cout << val << '\n';

        } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, int>) {
          std::ostringstream oss;
          oss << val;
          std::cout << oss.str() << '\n';

        } else if constexpr (std::is_same_v<T, Number>) {
          std::visit([](const auto& inner) -> void {
            if constexpr (std::is_same_v<std::decay_t<decltype(inner)>, double>) {
              std::cout << std::to_string(inner) << '\n';
            }
          }, val.get_value());
        }
      }, ast.value());
    }
  } while (input != "quit");
}