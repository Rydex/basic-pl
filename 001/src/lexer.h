#ifndef LEXER
#define LEXER

#include <string>
#include <vector>
#include <utility>
#include "exception.h"
#include "token.h"

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
	int position = -1;
	char cur_char = '\0';
	std::string text;
public:
	Lexer(const std::string& text);

	void advance();
	token_pair make_tokens();
	Token make_number();
};

token_pair run(const std::string& text);

#endif