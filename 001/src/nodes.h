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

#endif