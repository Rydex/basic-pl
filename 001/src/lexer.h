#ifndef LEXER
#define LEXER

#include <string>
#include <vector>
#include <utility>
#include "exception.h"
#include "token.h"
#include "position.h"
#include "parser.h"

const std::string PLS_T = "PLUS",
                  MIN_T = "MINUS",
                  DIV_T = "DIVIDE",
                  MUL_T = "MULTIPLY",
                  LPR_T = "LPAREN",
                  RPR_T = "RPAREN",
                  INT_T = "INT",
                  FLT_T = "FLOAT";

using token_pair = std::pair<std::vector<Token>, std::optional<Exception>>;

class Lexer {
private:
  std::string fn, text;
  Position pos{-1, 0, -1, fn, text};
  char cur_char = '\0';
public:
  Lexer(const std::string& fn, const std::string& text);

  void advance();
  token_pair make_tokens();
  Token make_number();
};

using ast_pair = std::pair<std::optional<NodeVariant>, std::optional<Exception>>;

ast_pair run(const std::string& fn, const std::string& text);

#endif