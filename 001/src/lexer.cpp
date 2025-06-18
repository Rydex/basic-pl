#include "lexer.h"
#include "context.h"
#include "parser.h"
#include "position.h"
#include "state/interpreter.h"
#include <algorithm>

Lexer::Lexer(const std::string& fn, const std::string& text): fn(fn), text(text) {
  advance();
}

void Lexer::advance() {
  pos.advance(cur_char);
  cur_char = (pos.get_idx() < (int)text.size()) ? text[pos.get_idx()] : '\0';
}

token_pair Lexer::make_tokens() {
  std::vector<Token> tokens{};

    while(cur_char != '\0') {
      if(cur_char == '\t' || cur_char == ' ') {
        advance();
      } else if (cur_char == '+') {
        tokens.emplace_back(PLS_T, std::nullopt, pos);
        advance();
      } else if (cur_char == '-') {
        tokens.emplace_back(MIN_T, std::nullopt, pos);
        advance();
      } else if (cur_char == '*') {
        tokens.emplace_back(MUL_T, std::nullopt, pos);
        advance();
      } else if (cur_char == '/') {
        tokens.emplace_back(DIV_T, std::nullopt, pos);
        advance();
      } else if (cur_char == '^') {
        tokens.emplace_back(POW_T, std::nullopt, pos);
        advance();
      } else if (cur_char == '%') {
        tokens.emplace_back(MOD_T, std::nullopt, pos);
        advance();
      } else if (cur_char == '=') {
        tokens.emplace_back(EQU_T, std::nullopt, pos);
        advance();
      } else if (cur_char == '(') {
        tokens.emplace_back(LPR_T, std::nullopt, pos);
        advance();
      } else if (cur_char == ')') {
        tokens.emplace_back(RPR_T, std::nullopt, pos);
        advance();
      } else if ((std::isdigit(cur_char) || cur_char == '.') && cur_char != '.') {
        tokens.emplace_back(make_number());
      } else if ((std::isalnum(cur_char) || cur_char == '_')) {
        tokens.emplace_back(make_identifier());
      } else {
        Position pos_start = pos.copy();
        char ch = cur_char;
        advance();
        return {
          std::vector<Token>{},
          std::make_shared<Exception>(IllegalCharException(pos_start, pos, ch))
        };
      }
    }

    tokens.emplace_back(EOF_T, std::nullopt, pos);
    return { tokens, nullptr };
}

bool Lexer::in_keywords(const std::string& text) {
  return std::find(KEYWORDS.cbegin(), KEYWORDS.cend(), text) != KEYWORDS.cend();
}

Token Lexer::make_number() {
  std::string num_str = "";
  int dot_count = 0;
  Position pos_start = pos.copy();

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

  return Token(
    dot_count == 0 ? INT_T : FLT_T,
    dot_count == 0 ? std::stoi(num_str) : std::stof(num_str),
    pos_start,
    pos
  );
}

Token Lexer::make_identifier() {
  std::string id_str = "";
  Position pos_start = pos.copy();

  while(cur_char != '\0' && (std::isalnum(cur_char) || cur_char == '_')) {
    id_str += cur_char;
    advance();
  }

  std::string type = ( in_keywords(id_str) ) ? KWD_T : ID_T;

  return Token(type, id_str, pos_start, pos);
}

RunType run(const std::string& fn, const std::string& text) {
  SymbolTable global;
  global.set("null", -1);
  global.set("quit", 0);

  Lexer lexer(fn, text);

  const auto&[tokens, error] = lexer.make_tokens();
  if(error) return { std::nullopt, error };

  // generate ast
  Parser parser(tokens);
  ParseResult ast = parser.parse();
  if(ast.error) return { std::nullopt, ast.error };

  Context context("<module>");
  context.symbol_table = std::make_shared<SymbolTable>(global);

  Interpreter interpreter;
  RTResult result = interpreter.visit(ast.node, context);

  if(result.error) {
    return { std::nullopt, result.error };
  }  

  return { result.value.value(), nullptr };
}