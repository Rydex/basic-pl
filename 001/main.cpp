#include <cctype>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <variant>
#include <vector>
#include <optional>

// position class
class Position {
private:
  int idx = 0; // index
  int ln = 0; // line number
  int col = 0; // column number
  std::string fn, ftxt; // file name, file text (contents)
public:
  Position(int idx, int ln, int col, const std::string& fn, const std::string& ftxt): idx(idx), ln(ln), col(col), fn(fn), ftxt(ftxt) {}

  Position& advance(char cur_char) {
    idx++;
    col++;

    if(cur_char == '\n') {
      ln++;
      col = 0;
    }

    return *this;
  }

  Position copy() {
    return Position(idx, ln, col, fn, ftxt);
  }

  int get_idx() const { return idx; }
  int get_ln() const { return ln; }
  int get_col() const { return col; }
  std::string get_fn() const { return fn; }
  std::string get_ftxt() const { return ftxt; }
};

// base class exception
class Exception {
private:
  std::string err_name, details;
  Position pos_start, pos_end;
public:
  Exception(const std::string& err_name, const std::string& details, const Position& pos_end, const Position& pos_start)
    : err_name(err_name), details(details), pos_start(pos_start), pos_end(pos_end) {}

  std::string as_string() const {
    std::string res = err_name + ": " + details;
    res += " File " + pos_start.get_fn() + ", line " + std::to_string(pos_start.get_ln() + 1);
    return res;
  }
};

class IllegalCharException : public Exception {
public:
  IllegalCharException(char details, const Position& pos_start, const Position& pos_end)
    : Exception("Illegal Character Exception", "'" + std::string(1, details) + "'", pos_start, pos_end) {}
};

// constants
const std::string INT_T = "INT";
const std::string FLT_T = "FLOAT";
const std::string MIN_T = "MINUS";
const std::string PLS_T = "PLUS";
const std::string MUL_T = "MULTIPLY";
const std::string DIV_T = "DIVIDE";
const std::string LPR_T = "LPAREN";
const std::string RPR_T = "RPAREN";

// struct for representing a none type
struct none_t {};

// struct for representing a value type
template<typename T>
struct value_t { 
  std::optional<T> value;
  // lparen token
  // int token : 123
};

// partially specialize value_t in the case if T is a none_t, value will be std::nullopt
template<>
struct value_t<none_t> {
  std::optional<none_t> value = std::nullopt;
};

using TokenValue = std::variant<none_t, int, double>;

// struct to represent tokens
template<typename Type>
struct Token {
  Type type;
  TokenValue value;

  std::string as_string() const {
    if (std::holds_alternative<none_t>(value)) {
      return type;
    } else if (std::holds_alternative<int>(value)) {
      return type + ':' + std::to_string(std::get<int>(value));
    } else {
      std::ostringstream oss;
      oss << std::get<double>(value);
      return type + ':' + oss.str();
    }
  }
};

// typedefs
using tokens_t = std::vector<Token<std::string>>;
using token_t = Token<std::string>;

//super long typedef
using make_tokens_t = std::pair<std::vector<Token<std::string>>, std::optional<Exception>>;
// nodes

struct NumberNode {
  token_t tok;

  std::string as_string() const {
    return tok.as_string();
  }
};

struct BinOpNode;

using node_t = std::variant<NumberNode, token_t, std::shared_ptr<BinOpNode>>;

struct BinOpNode {
  node_t left_node;
  token_t op_tok;
  node_t right_node;

  std::string as_string() const {
    // if(std::holds_alternative<token_t>(left_node) && std::holds_alternative<token_t>(right_node)) {
    //   return '(' + std::get<token_t>(left_node).as_string() + ", " + op_tok.as_string() + ", " + std::get<token_t>(right_node).as_string() + ')';
    // } else {
    //   return '(' + std::get<NumberNode>(left_node).as_string() + ", " + op_tok.as_string() + ", " + std::get<NumberNode>(right_node).as_string() + ')';
    // }

    auto node_to_str = [](const node_t& node) -> std::string {
      if(std::holds_alternative<NumberNode>(node)) {
        return std::get<NumberNode>(node).as_string();
      } else if (std::holds_alternative<token_t>(node)) {
        return std::get<token_t>(node).as_string();
      } else {
        return std::get<std::shared_ptr<BinOpNode>>(node)->as_string();
      }
    };

    return '(' + node_to_str(left_node) + ", " + op_tok.as_string() + ", " + node_to_str(right_node) + ')';
  }
};

// end of nodes

// parser class

class Parser {
private:
  int tok_idx = -1;
  tokens_t tokens;
  token_t cur_tok;
public:
  Parser(const tokens_t& tokens): tokens(tokens) {
    advance();
  }

  token_t advance() {
    tok_idx++;
    if(tok_idx < (int)tokens.size()) {
      cur_tok = tokens.at(tok_idx);
    }
    return cur_tok;
  }

  node_t factor() {
    token_t tok = cur_tok;

    if(tok.type == INT_T || tok.type == FLT_T) {
      advance();
      return NumberNode{tok};
    }
  }

  node_t term() {
    node_t left = factor();

    while(cur_tok.type == MUL_T || cur_tok.type == DIV_T) {
      token_t op_tok = cur_tok;
      advance();
      node_t right = factor();
      left = std::make_shared<BinOpNode>(left, op_tok, right);
    }

    return left;
  }

  node_t expr() {
    node_t left = term();

    while(cur_tok.type == PLS_T || cur_tok.type == MIN_T) {
      token_t op_tok = cur_tok;
      advance();
      node_t right = term();
      left = std::make_shared<BinOpNode>(left, op_tok, right);
    }

    return left;
  }

  node_t parse() {
    node_t res = expr();
    return res;
  }
};

// end of parser

// lexer class
class Lexer {
private:
  std::string text, fn;
  // Position* pos = new Position(-1, 0, -1);
  Position pos{-1, 0, -1, fn, text};
  char cur_char = '\0';

public:
  Lexer(const std::string& fn, const std::string& text): text(text), fn(fn) {
    advance();
  }

  void advance() {
    pos.advance(cur_char);
    cur_char = (pos.get_idx() < (int)text.length()) ? text[pos.get_idx()] : '\0';
  }

  make_tokens_t make_tokens() {
    make_tokens_t tokens;

    // lexer logic, check while current char isnt \0
    while(cur_char != '\0') {
      if(cur_char == ' ' || cur_char == '\t') { advance();
      } else if(cur_char == '+') {
        tokens.first.push_back(Token<std::string>{PLS_T, none_t{}});
        advance();
      } else if(cur_char == '-') {
        tokens.first.push_back(Token<std::string>{MIN_T, none_t{}});
        advance();
      } else if(cur_char == '/') {
        tokens.first.push_back(Token<std::string>{DIV_T, none_t{}});
        advance();
      } else if(cur_char == '*') {
        tokens.first.push_back(Token<std::string>{MUL_T, none_t{}});
        advance();
      } else if(cur_char == '(') {
        tokens.first.push_back(Token<std::string>{LPR_T, none_t{}});
        advance();
      } else if(cur_char == ')') {
        tokens.first.push_back(Token<std::string>{RPR_T, none_t{}});
        advance();
      } else if((std::isdigit(cur_char) || cur_char == '.') && cur_char != '.') {
        tokens.first.push_back(make_number());
      } else { // invalid charactre found
        Position pos_start = pos.copy();
        char ch = cur_char;
        advance();
        return std::make_pair(
          std::vector<Token<std::string>>(),
          IllegalCharException(ch, pos_start, pos)
        );
      }
    }
    return std::make_pair(
      tokens.first,
      std::nullopt
    );
  }

  Token<std::string> make_number() {
    std::string num_str = "";
    int dot_count = 0;

    while(cur_char != '\0' && (std::isdigit(cur_char) || cur_char == '.')) {
      if (cur_char == '.') {
        if (dot_count == 1) break;
        dot_count++;
        num_str += '.';
      } else {
        num_str += cur_char;
      }
      advance();
    }

    if(dot_count == 0) {
      return {INT_T, std::stoi(num_str)};
    } else {
      return {FLT_T, std::stof(num_str)};
    }
  }
};

std::pair<std::optional<node_t>, std::optional<Exception>> run(const std::string& fn, const std::string& text) {
  // lex the text
  Lexer lex(fn, text);
  // unpack into tokens and error
  const auto&[tokens, error] = lex.make_tokens();
  if(error) return { std::nullopt, error };

  // generate ast
  Parser parser(tokens);
  node_t ast = parser.parse();

  return { ast, std::nullopt };
}

int main() {
  std::string input;
  do {
    std::cout << "program (type exit to exit) > ";
    std::getline(std::cin, input);

    const auto&[ast, error] = run("<stdin>", input);

    if(error) {
      std::cout << error->as_string() << '\n';
    } else {
      // std::cout << '[';

      // for(size_t i=0; i<tokens.size(); i++) {
      //   std::cout << tokens[i].as_string();
      //   if(i != tokens.size() - 1) {
      //     std::cout << ", ";
      //   }
      // }
      // std::cout << "]\n";

      if(ast) {
        if(std::holds_alternative<NumberNode>(ast.value())) {
          std::cout << std::get<NumberNode>(ast.value()).as_string() << '\n';
        } else if (std::holds_alternative<token_t>(ast.value())) {
          std::cout << std::get<token_t>(ast.value()).as_string() << '\n';
        } else {
          std::cout << std::get<std::shared_ptr<BinOpNode>>(ast.value())->as_string() << '\n';
        }
      }
    }
  } while (input != "exit");
}