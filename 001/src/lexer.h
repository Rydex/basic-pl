#ifndef LEXER
#define LEXER

#include <string>
#include <vector>
#include <utility>
#include "exception.h"
#include "state/interpreter.h"
#include "state/symbol_table.h"
#include "token.h"
#include "position.h"
#include "parser.h"

const std::string PLS_T = "PLUS",
                  MIN_T = "MINUS",
                  DIV_T = "DIVIDE",
                  MUL_T = "MULTIPLY",
                  ID_T  = "IDENTIFIER",
                  KWD_T = "KEYWORD",
                  LPR_T = "LPAREN",
                  RPR_T = "RPAREN",
                  INT_T = "INT",
                  FLT_T = "FLOAT",
                  EOF_T = "EOF",
                  EQU_T = "EQUALS",
                  POW_T = "POWER",
                  MOD_T = "MODULUS",
                  EE_T  = "EE",
                  NE_T  = "NE",
                  LT_T  = "LT",
                  GT_T  = "GT",
                  LTE_T = "LTE",
                  GTE_T = "GTE";

const std::vector<std::string> KEYWORDS = {
  "var",
  "and",
  "or",
  "not",
  "if",
  "then",
  "elif",
  "else"
};

using VectorPair = std::pair<std::vector<Token>, std::shared_ptr<Exception>>;
using TokenPair = std::pair<std::optional<Token>, std::shared_ptr<Exception>>;

class Lexer {
private:
  std::string fn, text;
  Position pos{-1, 0, -1, fn, text};
  char cur_char = '\0';

public:
  Lexer(const std::string& fn, const std::string& text);


  bool in_keywords(const std::string& text);
  void advance();
  VectorPair make_tokens();
  Token make_number();
  Token make_identifier();
  TokenPair make_not_equals();
  Token make_equals();
  Token make_lt();
  Token make_gt();
};

using RunType = std::pair<std::optional<RTVariant>, std::shared_ptr<Exception>>;

RunType run(
  const std::string& fn,
  const std::string& text
);

#endif