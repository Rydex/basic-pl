#ifndef NODES
#define NODES
#include "position.h"
#include "token.h"
#include <variant>
#include <optional>
#include <memory>
#include <vector>

struct BinOpNode;
struct UnaryOpNode;
struct VarAssignNode;
struct VarAccessNode;

class RTResult;
class Interpreter;
class Context;

// helpers
using SharedBin = std::shared_ptr<BinOpNode>;
using SharedUnary = std::shared_ptr<UnaryOpNode>;
using SharedAssign = std::shared_ptr<VarAssignNode>;

// abstract base class
struct ASTNode {
  virtual RTResult accept(const Interpreter& visitor, Context& context) = 0; // visitor
  virtual Position get_pos_start() const = 0;
  virtual Position get_pos_end() const = 0;
  virtual ~ASTNode() {}
};

struct NumberNode : public ASTNode {
  std::optional<Token> tok;
  Position pos_start = tok->pos_start.value();
  Position pos_end = tok->pos_end.value();

  NumberNode(const Token& token)
  : tok(token), pos_start(token.pos_start.value()), pos_end(token.pos_end.value()) {};

  RTResult accept(const Interpreter& visitor, Context& context) override ;

  inline Position get_pos_start() const override { return pos_start; }
  inline Position get_pos_end() const override { return pos_end; }
};

struct VarAccessNode : public ASTNode {
  Token var_name_tok;

  Position pos_start = var_name_tok.pos_start.value();
  Position pos_end = var_name_tok.pos_end.value();

  VarAccessNode(const Token& token): var_name_tok(token) {}

  RTResult accept(const Interpreter& visitor, Context& context) override;

  inline Position get_pos_start() const override { return pos_start; }
  inline Position get_pos_end() const override { return pos_end; }
};

struct VarAssignNode : public ASTNode {
  Token var_name_tok;
  std::shared_ptr<ASTNode> value_node;

  Position pos_start = var_name_tok.pos_start.value();
  Position pos_end = var_name_tok.pos_end.value();

  VarAssignNode(const Token& tok, const std::shared_ptr<ASTNode>& node)
    : var_name_tok(tok), value_node(node) {}

  RTResult accept(const Interpreter& visitor, Context& context) override;

  inline Position get_pos_start() const override { return pos_start; }
  inline Position get_pos_end() const override { return pos_end; }
};


struct BinOpNode : public ASTNode {
  std::shared_ptr<ASTNode> left_node;
  Token op_tok;
  std::shared_ptr<ASTNode> right_node;
  std::optional<Position> pos_start, pos_end;
  
  BinOpNode(
    const std::shared_ptr<ASTNode>& left_node,
    const Token& op_tok,
    const std::shared_ptr<ASTNode>& right_node
  );

  RTResult accept(const Interpreter& visitor, Context& context) override;

  inline Position get_pos_start() const override { return pos_start.value(); }
  inline Position get_pos_end() const override { return pos_end.value(); }
};

struct UnaryOpNode : public ASTNode {
  Token op_tok;
  std::shared_ptr<ASTNode> node;
  std::optional<Position> pos_start = op_tok.pos_start.value(), pos_end;

  UnaryOpNode(
    const Token& op_tok,
    const std::shared_ptr<ASTNode>& node
  );

  RTResult accept(const Interpreter& visitor, Context& context) override;

  inline Position get_pos_start() const override { return pos_start.value(); }
  inline Position get_pos_end() const override { return pos_end.value(); }
};

struct IfNode : public ASTNode {
  std::vector<
    std::pair<std::shared_ptr<ASTNode>, std::shared_ptr<ASTNode>>
  > cases;
  std::shared_ptr<ASTNode> else_case;

  Position pos_start, pos_end;

  IfNode(
    const std::vector<std::pair<std::shared_ptr<ASTNode>, std::shared_ptr<ASTNode>>>& cases,
    const std::shared_ptr<ASTNode>& else_case
  )
    : cases(cases), else_case(else_case),
      pos_start(cases.at(0).first->get_pos_start()),
      pos_end(else_case ? else_case->get_pos_end() : cases.back().first->get_pos_end()) {}

  RTResult accept(const Interpreter& visitor, Context& context) override;

  inline Position get_pos_start() const override { return pos_start; }
  inline Position get_pos_end() const override { return pos_end; }
};

struct ForNode : public ASTNode {
  Token var_name_tok;
  std::shared_ptr<ASTNode> start_value, end_value, step_value, body;
  Position pos_start, pos_end;

  ForNode(
    const Token& var_name_tok,
    const std::shared_ptr<ASTNode>& start_value,
    const std::shared_ptr<ASTNode>& end_value,
    const std::shared_ptr<ASTNode>& step_value,
    const std::shared_ptr<ASTNode>& body
  )
    : var_name_tok(var_name_tok), start_value(start_value),
    end_value(end_value), step_value(step_value), body(body),
    pos_start(var_name_tok.pos_start.value()), pos_end(body->get_pos_end()) {}

  RTResult accept(const Interpreter& visitor, Context& context) override;

  inline Position get_pos_start() const override { return pos_start; }
  inline Position get_pos_end() const override { return pos_end; }
};

struct WhileNode : public ASTNode {
  std::shared_ptr<ASTNode> condition, body;
  Position pos_start, pos_end;

  WhileNode(
    const std::shared_ptr<ASTNode>& condition,
    const std::shared_ptr<ASTNode>& body
  )
    : condition(condition), body(body),
    pos_start(condition->get_pos_start()), pos_end(body->get_pos_end()) {}

  RTResult accept(const Interpreter& visitor, Context& context) override;

  inline Position get_pos_start() const override { return pos_start; }
  inline Position get_pos_end() const override { return pos_end; }
};

struct FuncDefNode : public ASTNode {
  std::optional<Token> var_name_tok;
  std::vector<Token> arg_name_toks;
  std::shared_ptr<ASTNode> body;
  std::optional<Position> pos_start, pos_end;

  FuncDefNode(
    const std::optional<Token>& var_name_tok,
    const std::vector<Token>& arg_name_toks,
    const std::shared_ptr<ASTNode>& body
  )
    : var_name_tok(var_name_tok), arg_name_toks(arg_name_toks),
    body(body), pos_start(std::nullopt), pos_end(std::nullopt) {
      if(var_name_tok) {
        pos_start = var_name_tok->pos_start.value();
      } else if(static_cast<int>(arg_name_toks.size()) > 0) {
        pos_start = arg_name_toks.at(0).pos_start.value();
      } else {
        pos_start = body->get_pos_start();
      }

      pos_end = body->get_pos_end();
    }

  RTResult accept(const Interpreter& visitor, Context& context) override;

  inline Position get_pos_start() const override { return pos_start.value(); }
  inline Position get_pos_end() const override { return pos_end.value(); }
};

struct CallNode : public ASTNode {
  std::shared_ptr<ASTNode> to_call;
  std::vector<std::shared_ptr<ASTNode>> arguments;
  Position pos_start, pos_end;

  CallNode(
    const std::shared_ptr<ASTNode>& to_call,
    const std::vector<std::shared_ptr<ASTNode>>& arguments
  )
    : to_call(to_call), arguments(arguments),
    pos_start(to_call->get_pos_start()),
    pos_end(static_cast<int>(arguments.size()) > 0 ? arguments.back()->get_pos_end()
                                                   : to_call->get_pos_end())
    {} // yes i know this code is horrible but plz bear with me

  RTResult accept(const Interpreter& visitor, Context& context) override;

  inline Position get_pos_start() const override { return pos_start; }
  inline Position get_pos_end() const override { return pos_end; }
};

#endif