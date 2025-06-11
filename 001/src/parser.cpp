#include "parser.h"
#include "exception.h"
#include "lexer.h"
#include <variant>

// nodes

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

// end nodes

// parse result

RegisterVariant ParseResult::register_(const RegisterVariant& res) {
  if(std::holds_alternative<ParseResult>(res)) { // check if res is another parseresult
    const ParseResult& other = std::get<ParseResult>(res);

    if(other.error) this->error = other.error;
    if(std::holds_alternative<NumberNode>(other.node.value())) {
      return std::get<NumberNode>(other.node.value());
    } else {
      return std::get<std::shared_ptr<BinOpNode>>(other.node.value());
    }
  }

  return res;
}

ParseResult& ParseResult::success(const NodeVariant& node) {
  this->node = node;
  return *this;
}

ParseResult& ParseResult::failure(const std::optional<Exception>& error) {
  this->error = error;
  return *this;
}

// end parse result

// parser

Parser::Parser(const std::vector<Token>& tokens): tokens(tokens), cur_tok(std::nullopt) {
  advance();
}

Token Parser::advance() {
  tok_idx++;
  if(tok_idx < (int)tokens.size()) {
    cur_tok = tokens.at(tok_idx);
  } else {
    cur_tok = Token(EOF_T, std::nullopt, tokens.back().pos_end.value());
  }

  return cur_tok.value();
}

ParseResult Parser::parse() {
  ParseResult res = expr();

  if(!res.error && cur_tok->type != EOF_T) {
    return res.failure(InvalidSyntaxException(
      cur_tok->pos_start.value(), cur_tok->pos_end.value(),
      "expected '+', '-', '*' or '/'"
    ));
  }

  return res;
}

ParseResult Parser::factor() {
  ParseResult res;
  Token tok = cur_tok.value();

  if(tok.type == INT_T || tok.type == FLT_T) {
    res.register_(advance());
    return res.success(NumberNode{tok});
  }

  return res.failure(InvalidSyntaxException(
    tok.pos_start.value(), tok.pos_end.value(),
    "expected int or float"
  ));
}

ParseResult Parser::term() {
  return bin_op({ MUL_T, DIV_T }, [this]() { return factor(); });
}

ParseResult Parser::expr() {
  return bin_op({ PLS_T, MIN_T }, [this]() { return term(); });
}

ParseResult Parser::bin_op(
  const std::vector<std::string>& ops,
  const std::function<ParseResult()>& func
) {
  ParseResult res;
  RegisterVariant func_register = res.register_(func());
  std::variant<NodeVariant, ParseResult> left;

  if(std::holds_alternative<ParseResult>(func_register))
    left = std::get<ParseResult>(func_register);

  if(res.error) return res;

  for(size_t i=0; i<ops.size(); i++) {
    while(cur_tok && cur_tok->type == ops.at(i)) {
      Token op_tok = cur_tok.value();
      res.register_(advance());
      std::variant<NodeVariant, ParseResult> right;

      if(std::holds_alternative<ParseResult>(func_register))
        right = std::get<ParseResult>(func_register);

      if(res.error) return res;

      if(std::holds_alternative<NodeVariant>(left) &&
        std::holds_alternative<NodeVariant>(right)) {
        left = std::make_shared<BinOpNode>(
          std::get<NodeVariant>(left),
          op_tok,
          std::get<NodeVariant>(right)
        );
      }
    }
  }

  return res.success(std::get<NodeVariant>(left));
}

// end parser