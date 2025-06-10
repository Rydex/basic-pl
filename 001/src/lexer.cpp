#include "lexer.h"
#include "parser.h"

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
        Position pos_start = pos.copy();
        char ch = cur_char;
        advance();
        return { std::vector<Token>{}, IllegalCharException(pos_start, pos, ch) };
      }
    }

    return { tokens, std::nullopt };
}

Token Lexer::make_number() {
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

ast_pair run(const std::string& fn, const std::string& text) {
	Lexer lexer(fn, text);

  const auto&[tokens, error] = lexer.make_tokens();
  if(error) return { std::nullopt, error };

  Parser parser(tokens);
  NodeVariant ast = parser.parse();

  return { ast, std::nullopt };
}