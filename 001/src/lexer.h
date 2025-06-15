#ifndef LEXER
#define LEXER

#include <string>
#include <vector>
#include <utility>
#include "exception.h"
#include "interpreter.h"
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
                  MOD_T = "MODULUS";

const std::vector<std::string> KEYWORDS = {
  "VAR"
};

using token_pair = std::pair<std::vector<Token>, std::shared_ptr<Exception>>;

class Lexer {
private:
  std::string fn, text;
  Position pos{-1, 0, -1, fn, text};
  char cur_char = '\0';

public:
  Lexer(const std::string& fn, const std::string& text);


  bool in_keywords(const std::string& text);
  void advance();
  token_pair make_tokens();
  Token make_number();
  Token make_identifier();
};

using RunType = std::pair<std::optional<Number>, std::shared_ptr<Exception>>;

RunType run(const std::string& fn, const std::string& text);

#endif