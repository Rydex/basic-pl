#ifndef NODES
#define NODES

#include "token.h"
#include <variant>
#include <optional>
#include <memory>

struct NumberNode {
  std::optional<Token> tok;
  Position pos_start = tok->pos_start.value();
  Position pos_end = tok->pos_end.value();

  NumberNode(const Token& token);

  std::string as_string() const;
};

struct BinOpNode;
struct UnaryOpNode;
struct VarAssignNode;
struct VarAccessNode;

// helpers
using SharedBin = std::shared_ptr<BinOpNode>;
using SharedUnary = std::shared_ptr<UnaryOpNode>;
using SharedAssign = std::shared_ptr<VarAssignNode>;

using NodeVariant = std::variant<
  NumberNode,
  SharedUnary,
  SharedBin,
  SharedAssign,
  VarAccessNode
>;

std::string stringify_node(const NodeVariant& node);

struct VarAccessNode {
  Token var_name_tok;

  Position pos_start = var_name_tok.pos_start.value();
  Position pos_end = var_name_tok.pos_end.value();
};

struct VarAssignNode {
  Token var_name;
  NodeVariant value_node;
};


struct BinOpNode {
  NodeVariant left_node;
  Token op_tok;
  NodeVariant right_node;
  std::optional<Position> pos_start, pos_end;
  
  BinOpNode(
    const NodeVariant& left_node,
    const Token& op_tok,
    const NodeVariant& right_node
  );

  std::string as_string() const;
};

struct UnaryOpNode {
  Token op_tok;
  NodeVariant node;
  std::optional<Position> pos_start = op_tok.pos_start.value(), pos_end;

  UnaryOpNode(
    const Token& op_tok,
    const NodeVariant& node
  );

  std::string as_string() const;
};

#endif