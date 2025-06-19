#include "parser.h"
#include "exception.h"
#include "lexer.h"
#include "nodes.h"
#include <algorithm>
#include <memory>

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

  ParseResult node_res = bin_op([this]() { return term(); }, { PLS_T, MIN_T });
  std::shared_ptr<ASTNode> node = res.register_(node_res);

  if(res.error) { return res; }

  return res.success(node);
}

ParseResult Parser::bin_op(
  const std::function<ParseResult()>& func_a,
  const std::vector<std::string>& ops,
  const std::optional<std::function<ParseResult()>>& func_b
) {
  std::function<ParseResult()> other_func = func_b.value_or(func_a);

  ParseResult res; // parseresult object
  ParseResult left_res = func_a(); // parseresult extracted from func_a()

  if(left_res.error) return left_res; // check if theres an error and if yes, return early
  std::shared_ptr<ASTNode> left = left_res.node; // extract node from left_res

   while(cur_tok && std::find(ops.begin(), ops.end(), cur_tok->type)!=ops.end()) { // check while cur_tok exists and
      // the type is in the vector
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