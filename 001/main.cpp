#include <cctype>
#include <iostream>
#include <optional>
#include <sstream>
#include <variant>
#include <vector>
#include <utility>

using TokenValue = std::variant<int, double>;

struct Token {
  std::string type;
  std::optional<TokenValue> value;

  Token(const std::string& type, const std::optional<TokenValue>& value = std::nullopt)
    :type(type), value(value) {}

  std::string as_string() const {
    if(!value) {
      return type;
    } else if(std::holds_alternative<int>(value.value())) {
      return type + ':' + std::to_string(std::get<int>(value.value()));
    } else {
      std::ostringstream oss;
      oss << std::get<double>(value.value());
      return type + ':' + oss.str();
    }
  }
};

class Exception {
private:
  std::string message, details;
public:
  Exception(const std::string& message, const std::string& details)
    : message(message), details(details) {}

  std::string as_string() const {
    return message + ": " + details;
  }
};

class IllegalCharException : public Exception {
public:
  IllegalCharException(char ch)
    : Exception("Illegal Character", "'" + std::string(1, ch) + "'") {}
};

const std::string PLS_T = "PLUS",
                  MIN_T = "MINUS",
                  DIV_T = "DIVIDE",
                  MUL_T = "MULTIPLY",
                  LPR_T = "LPAREN",
                  RPR_T = "RPAREN",
                  INT_T = "INT",
                  FLT_T = "FLOAT";

class Lexer {
private:
  int position = -1;
  char cur_char = '\0';
  std::string text;
public:
  Lexer(const std::string& text): text(text) {
    advance();
  }

  void advance() {
    position++;
    cur_char = (position < (int)text.size()) ? text[position] : '\0';
  }

  std::pair<std::vector<Token>, std::optional<Exception>> make_tokens() {
    std::vector<Token> tokens{};

    while(cur_char != '\0') {
      if(cur_char == '\t' || cur_char == ' ') {
        advance();
      } else if (cur_char == '+') {
        tokens.push_back(Token(PLS_T));
        advance();
      } else if (cur_char == '-') {
        tokens.push_back(Token(MIN_T));
        advance();
      } else if (cur_char == '*') {
        tokens.push_back(Token(MUL_T));
        advance();
      } else if (cur_char == '/') {
        tokens.push_back(Token(DIV_T));
        advance();
      } else if (cur_char == '(') {
        tokens.push_back(Token(LPR_T));
        advance();
      } else if (cur_char == ')') {
        tokens.push_back(Token(RPR_T));
        advance();
      } else if ((std::isdigit(cur_char) || cur_char == '.') && cur_char != '.') {
        tokens.push_back(make_number());
      } else {
        char ch = cur_char;
        advance();
        return { std::vector<Token>{}, IllegalCharException(ch) };
      }
    }

    return { tokens, std::nullopt };
  }

  Token make_number() {
    std::string num_str = "";
    int dot_count = 0;

    while(cur_char != '\0' && (std::isdigit(cur_char) || cur_char == '.')) {
      if(cur_char == '.') {
        if(dot_count == 1) break;
        dot_count++;

        num_str += '.';
      } else {
        num_str += cur_char;
      }

      advance();
    }

    if(dot_count == 0) {
      return Token(INT_T, std::stoi(num_str));
    } else {
      return Token(FLT_T, std::stof(num_str));
    }
  }
};

std::pair<std::vector<Token>, std::optional<Exception>> run(const std::string& text) {
  Lexer lexer(text);

  const auto&[tokens, error] = lexer.make_tokens();

  return { tokens, error };
}

int main() {
  std::string input;
  do {
    std::cout << "program (type exit to exit) > ";
    std::getline(std::cin, input);

    const auto&[tokens, error] = run(input);

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