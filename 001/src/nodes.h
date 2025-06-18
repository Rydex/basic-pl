#ifndef NODES
#define NODES
#include "state/interpreter.h"
#include "token.h"
#include <variant>
#include <optional>
#include <memory>

struct BinOpNode;
struct UnaryOpNode;
struct VarAssignNode;
struct VarAccessNode;

// helpers
using SharedBin = std::shared_ptr<BinOpNode>;
using SharedUnary = std::shared_ptr<UnaryOpNode>;
using SharedAssign = std::shared_ptr<VarAssignNode>;

struct ASTNode {
  virtual RTResult accept(const Interpreter& visitor, const Context& context) = 0;
  virtual ~ASTNode() {}
};

struct NumberNode : public ASTNode {
  std::optional<Token> tok;
  Position pos_start = tok->pos_start.value();
  Position pos_end = tok->pos_end.value();

  NumberNode(const Token& token)
  : tok(token), pos_start(token.pos_start.value()), pos_end(token.pos_end.value()) {};

  RTResult accept(const Interpreter& visitor, const Context& context) override ;
};

struct VarAccessNode : public ASTNode {
  Token var_name_tok;

  Position pos_start = var_name_tok.pos_start.value();
  Position pos_end = var_name_tok.pos_end.value();

  RTResult accept(const Interpreter& visitor, const Context& context) override;
};

struct VarAssignNode : public ASTNode {
  Token var_name_tok;
  std::shared_ptr<ASTNode> value_node;

  Position pos_start = var_name_tok.pos_start.value();
  Position pos_end = var_name_tok.pos_end.value();

  RTResult accept(const Interpreter& visitor, const Context& context) override;
};


struct BinOpNode : public ASTNode {
  std::shared_ptr<ASTNode> left_node;
  Token op_tok;
  std::shared_ptr<ASTNode> right_node;
  std::optional<Position> pos_start, pos_end;
  
  BinOpNode(
    const ASTNode& left_node,
    const Token& op_tok,
    const ASTNode& right_node
  );

  RTResult accept(const Interpreter& visitor, const Context& context) override;
};

struct UnaryOpNode : public ASTNode {
  Token op_tok;
  std::shared_ptr<ASTNode> node;
  std::optional<Position> pos_start = op_tok.pos_start.value(), pos_end;

  UnaryOpNode(
    const Token& op_tok,
    const ASTNode& node
  );

  RTResult accept(const Interpreter& visitor, const Context& context) override;
};

#endif