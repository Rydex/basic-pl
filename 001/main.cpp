#include <iostream>
#include <sstream>
#include "src/lexer.h"


auto handle_number = [](const auto& inner) -> void {
  if constexpr (std::is_same_v<std::decay_t<decltype(inner)>, double>) {
    std::ostringstream oss;
    oss << inner;
    std::cout << oss.str() << '\n';
  }
};

auto handle_nodes = [](const auto& val) -> void {
  using T = std::decay_t<decltype(val)>;

  if constexpr (std::is_same_v<T, std::string>) {
    std::cout << val << '\n';
  } else if constexpr (std::is_same_v<T, Number>) {
    std::visit(handle_number, val.get_value());
  }
};

int main() {
  std::string input;

  do {
    std::cout << "program (type quit to quit) > ";
    std::getline(std::cin, input);
    const auto&[ast, error] = run("<stdin>", input);

    if(error) { // if theres an error print it as string
      std::cout << error->as_string() << '\n'; 
    } else {

      std::visit(handle_nodes, ast.value());
    }
  } while (input != "quit");
}