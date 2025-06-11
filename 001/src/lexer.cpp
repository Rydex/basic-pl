#include "lexer.h"
#include "parser.h"
#include "position.h"

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
      } else if (cur_char == '(') {
        tokens.emplace_back(LPR_T, std::nullopt, pos);
        advance();
      } else if (cur_char == ')') {
        tokens.emplace_back(RPR_T, std::nullopt, pos);
        advance();
      } else if ((std::isdigit(cur_char) || cur_char == '.') && cur_char != '.') {
        tokens.emplace_back(make_number());
      } else {
        Position pos_start = pos.copy();
        char ch = cur_char;
        advance();
        return { std::vector<Token>{}, IllegalCharException(pos_start, pos, ch) };
      }
    }

    tokens.push_back(Token(EOF_T));
    return { tokens, std::nullopt };
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

  if(dot_count == 0) {
    return Token(INT_T, std::stoi(num_str), pos_start, pos);
  } else {
    return Token(FLT_T, std::stof(num_str), pos_start, pos);
  }
}

ASTPair run(const std::string& fn, const std::string& text) {
  Lexer lexer(fn, text);

  const auto&[tokens, error] = lexer.make_tokens();
  if(error) return { std::nullopt, error };

  Parser parser(tokens);
  NodeVariant ast = parser.parse();

  return { ast, std::nullopt };
}