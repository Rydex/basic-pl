#include "parser.h"
#include "exception.h"
#include "lexer.h"
#include <variant>

// nodes

std::string stringify_node(const NodeVariant& node) {
  return std::visit([](const auto& val) -> std::string {
    if constexpr (std::is_same_v<std::decay_t<decltype(val)>, NumberNode>) {
      return val.as_string();
    } else {
      return val->as_string();
    }
  }, node);
}

std::string UnaryOpNode::as_string() const {
  return '(' + op_tok.as_string() + ", " + stringify_node(node) + ')';
}

UnaryOpNode::UnaryOpNode(
  const Token& op_tok,
  const NodeVariant& node
): op_tok(op_tok), node(node) {
  pos_end = std::visit([](const auto& val) -> Position {
    if constexpr(std::is_same_v<std::decay_t<decltype(val)>, NumberNode>) {
      return val.pos_end;
    } else {
      return val->pos_end.value();
    }
  }, node);
}

std::string NumberNode::as_string() const {
  return tok->as_string();
}

std::string BinOpNode::as_string() const {
  return '(' + stringify_node(left_node) + ", " + op_tok.as_string() + ", "
             + stringify_node(right_node) + ')';
}

Position get_pos_end(const NodeVariant& node) {
  return std::visit([](const auto& val) -> Position {
    if constexpr (std::is_same_v<std::decay_t<decltype(val)>, NumberNode>) {
      return val.pos_end;
    } else {
      return val->pos_end.value();
    }
  }, node);
}

Position get_pos_start(const NodeVariant& node) {
  return std::visit([](const auto& val) -> Position {
    if constexpr (std::is_same_v<std::decay_t<decltype(val)>, NumberNode>) {
      return val.pos_start;
    } else {
      return val->pos_start.value();
    }
  }, node);
}

BinOpNode::BinOpNode(
  const NodeVariant& left_node,
  const Token& op_tok,
  const NodeVariant& right_node
): left_node(left_node), op_tok(op_tok), right_node(right_node) {
  pos_start = get_pos_start(left_node);
  pos_end = get_pos_end(right_node);
}

// end nodes

// parse result

RegisterVariant ParseResult::register_(const RegisterVariant& res) {
  std::visit([this](const auto& val) -> RegisterVariant {
    using T = std::decay_t<decltype(val)>;

    if constexpr (std::is_same_v<T, ParseResult>) {
      if(val.error) this->error = val.error;

      return std::visit([](const auto& inner_node) -> RegisterVariant {
        return inner_node;
      }, val.node.value());
    }
    
    return val;
  }, res);

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

  if(tok.type == PLS_T || tok.type == MIN_T) {
    res.register_(advance());
    ParseResult factor_res = factor();

    if(factor_res.error) return factor_res;

    return res.success(std::make_shared<UnaryOpNode>(tok, factor_res.node.value()));

  } else if(tok.type == INT_T || tok.type == FLT_T) {
    res.register_(advance());
    return res.success(NumberNode{tok});
  } else if(tok.type == LPR_T) {
    res.register_(advance());
    ParseResult expr_res = expr();

    if(expr_res.error) return expr_res;

    if(cur_tok->type == RPR_T) {
      res.register_(advance());
      return res.success(expr_res.node.value());
    } else {
      return res.failure(InvalidSyntaxException(
        cur_tok->pos_start.value(), cur_tok->pos_end.value(),
        "expected ')'"
      ));
    }
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
  ParseResult res; // parseresult object
  ParseResult left_res = func(); // parseresult extracted from func()

  if(left_res.error) return left_res; // check if theres an error and if yes, return early
  NodeVariant left = left_res.node.value(); // extract node from left_res

  for(size_t i=0; i<ops.size(); i++) {
    while(cur_tok && cur_tok->type == ops.at(i)) { // check while cur_tok exists and
      // the type is in the vector
      Token op_tok = cur_tok.value(); // get operator token which is just current token
      res.register_(advance()); // advance
      ParseResult right_res = func(); // parseresult extracted from func()

      if(right_res.error) return right_res; // if theres an error return early

      NodeVariant right = right_res.node.value(); // extract node from right_res

      left = std::make_shared<BinOpNode>(left, op_tok, right); // finally, make
      // a shared binopnode pointer consisting of the 3 elements
    }
  }

  return res.success(left);
}

// end parser