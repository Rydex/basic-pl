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
  ParseResult res;
  res.node = res.register_(expr());

  if(!res.error && cur_tok->type != EOF_T) {
    return res.failure(std::make_shared<InvalidSyntaxException>(
      cur_tok->pos_start.value(), cur_tok->pos_end.value(),
      "expected '+', '-', '*', '/', '^', '==', '!=', '<', '>', <=', '>=', 'and' or 'or', got "
      + cur_tok->type
    ));
  }

  return res;
}

ParseResult Parser::power() {
  return bin_op([this]() { return atom(); }, { POW_T, MOD_T }, [this]() { return factor(); });
}

ParseResult Parser::if_expr() {
  ParseResult res;
  std::vector<
    std::pair<std::shared_ptr<ASTNode>, std::shared_ptr<ASTNode>>
  > cases = {};
  std::shared_ptr<ASTNode> else_case = nullptr;

  if(!cur_tok->matches(KWD_T, "if")) {
    return res.failure(std::make_shared<InvalidSyntaxException>(
      cur_tok->pos_start.value(), cur_tok->pos_end.value(),
      "expected 'if', got " + cur_tok->type
    ));
  }

  res.register_advance();
  advance();

  std::shared_ptr<ASTNode> condition = res.register_(expr());
  if(res.error) return res;

  if(!cur_tok->matches(KWD_T, "then")) {
    return res.failure(std::make_shared<InvalidSyntaxException>(
      cur_tok->pos_start.value(), cur_tok->pos_end.value(),
      "expected 'then' after 'if' expression got " + cur_tok->type
    ));
  }

  res.register_advance();
  advance();

  std::shared_ptr<ASTNode> expr_res = res.register_(expr());
  if(res.error) return res;

  cases.emplace_back(condition, expr_res);

  while(cur_tok->matches(KWD_T, "elif")) {
    res.register_advance();
    advance();

    condition = res.register_(expr());
    if(res.error) return res;

    if(!cur_tok->matches(KWD_T, "then")) {
      return res.failure(std::make_shared<InvalidSyntaxException>(
        cur_tok->pos_start.value(), cur_tok->pos_end.value(),
        "expected 'then' after 'elif' expression, got " + cur_tok->type
      ));
    }

    res.register_advance();
    advance();

    expr_res = res.register_(expr());
    if(res.error) return res;

    cases.emplace_back(condition, expr_res);
  }

  if(cur_tok->matches(KWD_T, "else")) {
    res.register_advance();
    advance();

    else_case = res.register_(expr());
    if(res.error) return res;
  }

  return res.success(std::make_shared<IfNode>(cases, else_case));
}

ParseResult Parser::for_expr() {
  ParseResult res;

  if(!cur_tok->matches(KWD_T, "for")) {
    return res.failure(std::make_shared<InvalidSyntaxException>(
      cur_tok->pos_start.value(), cur_tok->pos_end.value(),
      "expected 'for', got " + cur_tok->type
    ));
  }

  res.register_advance();
  advance();

  if(cur_tok->type != ID_T) {
    return res.failure(std::make_shared<InvalidSyntaxException>(
      cur_tok->pos_start.value(), cur_tok->pos_end.value(),
      "expected identifier after 'for', got " + cur_tok->type
    ));
  }

  Token var_name = cur_tok.value();
  res.register_advance();
  advance();

  if(cur_tok->type != EQU_T) {
    return res.failure(std::make_shared<InvalidSyntaxException>(
      cur_tok->pos_start.value(), cur_tok->pos_end.value(),
      "expected '=' after identifier, got " + cur_tok->type
    ));
  }

  res.register_advance();
  advance();

  std::shared_ptr<ASTNode> start_value = res.register_(expr());
  if(res.error) return res;

  if(!cur_tok->matches(KWD_T, "to")) {
    return res.failure(std::make_shared<InvalidSyntaxException>(
      cur_tok->pos_start.value(), cur_tok->pos_end.value(),
      "expected 'to' after equals, got " + cur_tok->type
    ));
  }

  res.register_advance();
  advance();

  std::shared_ptr<ASTNode> end_value = res.register_(expr());
  if(res.error) return res;

  std::shared_ptr<ASTNode> step_value;

  if(cur_tok->matches(KWD_T, "step")) {
    res.register_advance();
    advance();

    step_value = res.register_(expr());
    if(res.error) return res;
  } else {
    step_value = nullptr;
  }

  if(!cur_tok->matches(KWD_T, "do")) {
    return res.failure(std::make_shared<InvalidSyntaxException>(
      cur_tok->pos_start.value(), cur_tok->pos_end.value(),
      "expected 'do' after 'for' expression, got " + cur_tok->type
    ));
  }

  res.register_advance();
  advance();

  std::shared_ptr<ASTNode> body = res.register_(expr());
  if(res.error) return res;

  return res.success(std::make_shared<ForNode>(
    var_name, start_value, end_value, step_value, body
  ));
}

ParseResult Parser::while_expr() {
  ParseResult res;

  if(!cur_tok->matches(KWD_T, "while")) {
    return res.failure(std::make_shared<InvalidSyntaxException>(
      cur_tok->pos_start.value(), cur_tok->pos_end.value(),
      "expected 'while', got " + cur_tok->type
    ));
  }

  res.register_advance();
  advance();

  std::shared_ptr<ASTNode> condition = res.register_(expr());
  if(res.error) return res;

  if(!cur_tok->matches(KWD_T, "do")) {
    return res.failure(std::make_shared<InvalidSyntaxException>(
      cur_tok->pos_start.value(), cur_tok->pos_end.value(),
      "expected 'do' after condition, got " + cur_tok->type
    ));
  }

  res.register_advance();
  advance();

  std::shared_ptr<ASTNode> body = res.register_(expr());
  if(res.error) return res;

  return res.success(std::make_shared<WhileNode>(condition, body));
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
    res.register_(expr_res);

    if(res.error) return res;

    if(cur_tok->type == RPR_T) {
      res.register_advance();
      advance();
      return res.success(expr_res.node);

    } else {
      return res.failure(std::make_shared<InvalidSyntaxException>(
        cur_tok->pos_start.value(), cur_tok->pos_end.value(),
        "expected ')', got " + cur_tok->type
      ));
    }

  } else if(cur_tok->matches(KWD_T, "if")) {
    std::shared_ptr<ASTNode> if_expr_res = res.register_(if_expr());

    if(res.error) return res;
    return res.success(if_expr_res);

  } else if(cur_tok->matches(KWD_T, "for")) {
    std::shared_ptr<ASTNode> for_expr_res = res.register_(for_expr());

    if(res.error) return res;
    return res.success(for_expr_res);

  } else if(cur_tok->matches(KWD_T, "while")) {
    std::shared_ptr<ASTNode> while_expr_res = res.register_(while_expr());

    if(res.error) return res;
    return res.success(while_expr_res);
  }
  
  return res.failure(std::make_shared<InvalidSyntaxException>(
    tok.pos_start.value(), tok.pos_end.value(),
    "expected int, float, identifier, '+', '-' or '(', got " + tok.type
  ));
}

ParseResult Parser::factor() {
  ParseResult res;
  Token tok = cur_tok.value();

  if(tok.type == PLS_T || tok.type == MIN_T) {
    res.register_advance();
    advance();
    std::shared_ptr<ASTNode> node = res.register_(factor());

    if(res.error) return res;

    return res.success(std::make_shared<UnaryOpNode>(tok, node));
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

  if(cur_tok->matches(KWD_T, "not")) {
    op_tok = cur_tok;
    res.register_advance();
    advance();

    std::shared_ptr<ASTNode> node_expr = res.register_(bin_op(
      [this]() { return comp_expr(); }, { EE_T, NE_T, LT_T, GT_T, LTE_T, GTE_T }
    ));

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
    std::shared_ptr<ASTNode> other_expr = res.register_(expr());

    if(res.error) return res;

    // return res.success(VarAssignNode(var_name, other_expr));
    return res.success(std::make_shared<VarAssignNode>(var_name, other_expr));
  }

  ParseResult node = bin_op(
    [this]() { return comp_expr(); }, { {KWD_T, "and"}, {KWD_T, "or"} }
  );
  res.register_(node);

  if(res.error) { 
    return res.failure(std::make_shared<InvalidSyntaxException>(
      cur_tok->pos_start.value(), cur_tok->pos_end.value(),
      "expected 'var', int, float, identifier, '+', '-', '(' or 'not'"
    ));
  }

  return res.success(node.node);
}

ParseResult Parser::bin_op(
  const std::function<ParseResult()>& func_a,
  const std::vector<std::pair<std::string, std::string>>& ops,
  const std::optional<std::function<ParseResult()>>& func_b
) { // overload with { {tok_type, tok_value} }
  std::function<ParseResult()> other_func = func_b.value_or(func_a);

  ParseResult res; // parseresult object
  ParseResult left_res = func_a(); // parseresult extracted from func_a()

  std::shared_ptr<ASTNode> left = res.register_(left_res); // extract node from left_res
  if(res.error) return res; // check if theres an error and if yes, return early
  std::string target_name = cur_tok->type;

  while(
    cur_tok &&
    std::find_if(ops.begin(), ops.end(), [&](const auto& p) -> bool {
      std::optional<TokenValue> target_value = cur_tok->value;

      if(!target_value) return false;

      return std::visit([&](const auto& val) -> bool {
        if constexpr(std::is_same_v<std::decay_t<decltype(val)>, std::string>) {
          return p.first == cur_tok->type && p.second == val;
        } else {
          return false;
        }
      }, target_value.value());
    }) != ops.end()
  ) { // check while cur_tok exists and
    // the type/value is in the vector
    Token op_tok = cur_tok.value(); // get operator token which is just current token
    res.register_advance();
    advance(); // advance
    ParseResult right_res = other_func(); // parseresult extracted from func()
    std::shared_ptr<ASTNode> right = res.register_(right_res); // extract node from right_res

    if(res.error) return res; // if theres an error return early


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
  std::shared_ptr<ASTNode> left = res.register_(left_res); // extract node from left_res

  if(res.error) return res; // check if theres an error and if yes, return early

  while(
    std::find(ops.begin(), ops.end(), cur_tok->type) != ops.end()
  ) { // check while cur_tok exists and
      // the type/value is in the vector
      Token op_tok = cur_tok.value(); // get operator token which is just current token
      res.register_advance();
    advance(); // advance
      ParseResult right_res = other_func(); // parseresult extracted from func()
      std::shared_ptr<ASTNode> right = res.register_(right_res); // extract node from right_res

      if(res.error) return res; // if theres an error return early


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