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

const std::string PLS_T = "plus",
                  MIN_T = "minus",
                  DIV_T = "divide",
                  MUL_T = "multiply",
                  ID_T  = "identifier",
                  KWD_T = "keyword",
                  LPR_T = "lparen",
                  RPR_T = "rparen",
                  INT_T = "int",
                  FLT_T = "float",
                  EOF_T = "eof",
                  EQU_T = "equals",
                  POW_T = "power",
                  MOD_T = "modulus",
                  EE_T  = "double-equals",
                  NE_T  = "not-equal",
                  LT_T  = "less-than",
                  GT_T  = "greater-than",
                  LTE_T = "less-than-or-equal",
                  GTE_T = "greater-than-or-equal",
                  COM_T = "comma",
                  ARW_T = "arrow";

const std::vector<std::string> KEYWORDS = {
  "var",
  "and",
  "or",
  "not",
  "if",
  "then",
  "elif",
  "else",
  "for",
  "to",
  "step",
  "while",
  "do"
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
  Token make_minus_or_arrow();
};

using RunType = std::pair<std::optional<RTVariant>, std::shared_ptr<Exception>>;

RunType run(
  const std::string& fn,
  const std::string& text
);

#endif