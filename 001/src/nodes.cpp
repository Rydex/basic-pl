#include "nodes.h"
#include "utils/ast_utils.h"

std::string stringify_node(const NodeVariant& node) {
  return std::visit([](const auto& val) -> std::string {

    using T = std::decay_t<decltype(val)>;

    if constexpr (std::is_same_v<T, NumberNode>) {
      return val.as_string();
    } else if constexpr (std::is_same_v<T, SharedBin> || std::is_same_v<T, SharedUnary>) {
      return val->as_string();
    } else {
      return "<null>";
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
  pos_end = get_pos_end(node);
}

NumberNode::NumberNode(const Token& token)
  : tok(token), pos_start(token.pos_start.value()), pos_end(token.pos_end.value()) {}

std::string NumberNode::as_string() const {
  return tok->as_string();
}

std::string BinOpNode::as_string() const {
  return '(' + stringify_node(left_node) + ", " + op_tok.as_string() + ", "
             + stringify_node(right_node) + ')';
}

BinOpNode::BinOpNode(
  const NodeVariant& left_node,
  const Token& op_tok,
  const NodeVariant& right_node
): left_node(left_node), op_tok(op_tok), right_node(right_node) {
  pos_start = get_pos_start(left_node);
  pos_end = get_pos_end(right_node);
}