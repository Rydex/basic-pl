#include "parser.h"
#include "lexer.h"
#include <variant>

std::string NumberNode::as_string() const {
	return tok->as_string();
}

std::string BinOpNode::as_string() const {
	auto node_to_str = [](const NodeVariant& node) -> std::string {
		if(std::holds_alternative<NumberNode>(node)) {
			return std::get<NumberNode>(node).as_string();
		} else {
			return std::get<std::shared_ptr<BinOpNode>>(node)->as_string();
		}
	};

	return '(' + node_to_str(left_node) + ", " + op_tok.as_string() + ", "
						 + node_to_str(right_node) + ')';
}

Parser::Parser(const std::vector<Token>& tokens): cur_tok(std::nullopt) {
	advance();
}

Token Parser::advance() {
	tok_idx++;
	if(tok_idx < (int)tokens.size()) {
		cur_tok = tokens.at(tok_idx);
	}

	return cur_tok.value();
} 

NodeVariant Parser::parse() {
	NodeVariant res = expr();
	return res;
}

NodeVariant Parser::factor() {
	Token tok = cur_tok.value();

	if(tok.type == INT_T || tok.type == FLT_T) {
		advance();
		return NumberNode{tok};
	}
}

NodeVariant Parser::term() {
	NodeVariant left = factor();

	while(cur_tok->type == MUL_T || cur_tok->type == DIV_T) {
		Token op_tok = cur_tok.value();
		advance();
		NodeVariant right = factor();
		left = std::make_shared<BinOpNode>(left, op_tok, right);
	}

	return left;
}

NodeVariant Parser::expr() {
	NodeVariant left = term();

	while(cur_tok->type == PLS_T || cur_tok->type == MIN_T) {
		Token op_tok = cur_tok.value();
		advance();
		NodeVariant right = factor();
		left = std::make_shared<BinOpNode>(left, op_tok, right);
	}

	return left;
}