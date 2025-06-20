#include "parser.h"
#include "exception.h"
#include "lexer.h"
#include "nodes.h"
#include "token.h"
#include <algorithm>
#include <memory>
#include <string>
#include <type_traits>

// parse result

void ParseResult::register_advance() {
  advance_count++;
}

std::shared_ptr<ASTNode> ParseResult::register_(const ParseResult& res) {
  this->advance_count += res.advance_count;
  if(res.error) this->error = res.error;
  return res.node;
}

ParseResult& ParseResult::success(const std::shared_ptr<ASTNode>& node) {
  this->node = node;
  return *this;
}

ParseResult& ParseResult::failure(const std::shared_ptr<Exception>& error) {
  if(!this->error || advance_count == 0)
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
    return res.failure(std::make_shared<InvalidSyntaxException>(
      cur_tok->pos_start.value(), cur_tok->pos_end.value(),
      "expected '+', '-', '*', '/' or '^'"
    ));
  }

  return res;
}

ParseResult Parser::power() {
  return bin_op([this]() { return atom(); }, { POW_T, MOD_T }, [this]() { return factor(); });
}

ParseResult Parser::atom() {
  ParseResult res;
  Token tok = cur_tok.value();

  if(tok.type == INT_T || tok.type == FLT_T) {
    res.register_advance();
    advance();
    return res.success(std::make_shared<NumberNode>(tok));

  } else if(tok.type == ID_T) {
    res.register_advance();
    advance();
    return res.success(std::make_shared<VarAccessNode>(tok));

  } else if(tok.type == LPR_T) {
    res.register_advance();
    advance();
    ParseResult expr_res = expr();

    if(expr_res.error) return res;

    if(cur_tok->type == RPR_T) {
      res.register_advance();
      advance();
      return res.success(expr_res.node);

    } else {
      return res.failure(std::make_shared<InvalidSyntaxException>(
        cur_tok->pos_start.value(), cur_tok->pos_end.value(),
        "expected ')'"
      ));
    }
  }

  return res.failure(std::make_shared<InvalidSyntaxException>(
    tok.pos_start.value(), tok.pos_end.value(),
    "expected int, float, identifier, '+', '-' or '('"
  ));
}

ParseResult Parser::factor() {
  ParseResult res;
  Token tok = cur_tok.value();

  if(tok.type == PLS_T || tok.type == MIN_T) {
    res.register_advance();
    advance();
    ParseResult factor_res = factor();

    if(factor_res.error) return factor_res;

    return res.success(std::make_shared<UnaryOpNode>(tok, factor_res.node));
  }

  return power();
}

ParseResult Parser::term() {
  return bin_op([this]() { return factor(); }, { MUL_T, DIV_T });
}

ParseResult Parser::arith_expr() {
  return bin_op([this]() { return term(); }, { PLS_T, MIN_T });
}

ParseResult Parser::comp_expr() {
  ParseResult res;
  std::optional<Token> op_tok;

  if(cur_tok->matches(KWD_T, "NOT")) {
    op_tok = cur_tok;
    res.register_advance();
    advance();

    std::shared_ptr<ASTNode> node_expr = res.register_(comp_expr());
    if(res.error) return res;

    return res.success(std::make_shared<UnaryOpNode>(op_tok.value(), node_expr));
  }

  std::shared_ptr<ASTNode> node_expr = res.register_(bin_op(
    [this]() { return arith_expr(); }, { EE_T, NE_T, LT_T, GT_T, LTE_T, GTE_T }
  ));

  if(res.error)
    return res.failure(std::make_shared<InvalidSyntaxException>(
      cur_tok->pos_start.value(), cur_tok->pos_end.value(),
      "expected int, float, identifier, '+', '-', '(' or 'not'"
    ));

  return res.success(node_expr);
}

ParseResult Parser::expr() {
  ParseResult res;

  if(cur_tok->matches(KWD_T, "var")) {
    res.register_advance();
    advance();

    if(cur_tok->type != ID_T) {
      return res.failure(std::make_shared<InvalidSyntaxException>(
        cur_tok->pos_start.value(), cur_tok->pos_end.value(),
        "expected identifier after 'var'"
      ));
    }

    Token var_name = cur_tok.value();
    res.register_advance();
    advance();

    if(cur_tok->type != EQU_T) {
      return res.failure(std::make_shared<InvalidSyntaxException>(
        cur_tok->pos_start.value(), cur_tok->pos_end.value(),
        "expected '=' after identifier"
      ));
    }

    res.register_advance();
    advance();
    ParseResult other_expr = expr();
    res.register_(other_expr);

    if(res.error) return res;

    // return res.success(VarAssignNode(var_name, other_expr));
    return res.success(std::make_shared<VarAssignNode>(var_name, other_expr.node));
  }

  std::shared_ptr<ASTNode> node = res.register_(bin_op(
    [this]() { return comp_expr(); }, { {KWD_T, "AND"}, {KWD_T, "OR"} }
  ));

  if(res.error) { return res; }

  return res.success(node);
}

ParseResult Parser::bin_op(
  const std::function<ParseResult()>& func_a,
  const std::vector<std::pair<std::string, std::string>>& ops,
  const std::optional<std::function<ParseResult()>>& func_b
) { // overload with { {tok_type, tok_value} }
  std::function<ParseResult()> other_func = func_b.value_or(func_a);

  ParseResult res; // parseresult object
  ParseResult left_res = func_a(); // parseresult extracted from func_a()

  if(left_res.error) return left_res; // check if theres an error and if yes, return early
  std::shared_ptr<ASTNode> left = left_res.node; // extract node from left_res
  std::string target_name = cur_tok->type;
  TokenValue target_value = cur_tok->value.value();

  while(
    cur_tok &&
    std::find_if(ops.begin(), ops.end(), [&](const auto& p) -> bool {
      return std::visit([&](const auto& val) -> bool {
        if constexpr(std::is_same_v<std::decay_t<decltype(val)>, std::string>) {
          return p.first == cur_tok->type && p.second == val;
        } else {
          return false;
        }
      }, cur_tok->value.value());
    }) != ops.end()
  ) { // check while cur_tok exists and
      // the type/value is in the vector
      Token op_tok = cur_tok.value(); // get operator token which is just current token
      res.register_advance();
      advance(); // advance
      ParseResult right_res = other_func(); // parseresult extracted from func()

      if(right_res.error) return right_res; // if theres an error return early

      std::shared_ptr<ASTNode> right = right_res.node; // extract node from right_res

      left = std::make_shared<BinOpNode>(left, op_tok, right); // finally, make
      // a shared binopnode pointer consisting of the 3 elements
  }

  return res.success(left);
}

ParseResult Parser::bin_op(
    const std::function<ParseResult()>& func_a,
    const std::vector<std::string>& ops,
    const std::optional<std::function<ParseResult()>>& func_b
) { // normal
  std::function<ParseResult()> other_func = func_b.value_or(func_a);

  ParseResult res; // parseresult object
  ParseResult left_res = func_a(); // parseresult extracted from func_a()

  if(left_res.error) return left_res; // check if theres an error and if yes, return early
  std::shared_ptr<ASTNode> left = left_res.node; // extract node from left_res

  while(
    std::find(ops.begin(), ops.end(), cur_tok->type) != ops.end()
  ) { // check while cur_tok exists and
      // the type/value is in the vector
      Token op_tok = cur_tok.value(); // get operator token which is just current token
      res.register_advance();
    advance(); // advance
      ParseResult right_res = other_func(); // parseresult extracted from func()

      if(right_res.error) return right_res; // if theres an error return early

      std::shared_ptr<ASTNode> right = right_res.node; // extract node from right_res

      left = std::make_shared<BinOpNode>(left, op_tok, right); // finally, make
      // a shared binopnode pointer consisting of the 3 elements
  }

  return res.success(left);
}

Position get_pos_end(const std::shared_ptr<ASTNode>& node) {
  return node->get_pos_end();
}

Position get_pos_start(const std::shared_ptr<ASTNode>& node) {
  return node->get_pos_start();
}

// end parser