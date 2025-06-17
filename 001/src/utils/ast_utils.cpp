#include "ast_utils.h"

NodeVariant convert(const RegisterVariant& val) {
  return std::visit([](const auto& node) -> NodeVariant {
    using T = std::decay_t<decltype(node)>;

    if constexpr (
      std::is_same_v<T, NumberNode> ||
      std::is_same_v<T, SharedUnary> ||
      std::is_same_v<T, SharedBin> ||
      std::is_same_v<T, SharedAssign> ||
      std::is_same_v<T, VarAccessNode>
    ) {
      return node;
    } else {
      throw std::runtime_error("invalid type at ast_utils.cpp");
    }
  }, val);
}

Position get_pos_end(const NodeVariant& node) {
  return std::visit([&](const auto& val) -> Position {
    using T = std::decay_t<decltype(val)>;

    if constexpr (std::is_same_v<T, NumberNode>) {
      return val.pos_end;
    } else if constexpr (
      std::is_same_v<T, SharedBin> ||
      std::is_same_v<T, SharedUnary>
    ) {
      return val->pos_end.value();
    } else {
      throw std::runtime_error("get_pos_end: unhandled node type: " + std::to_string(node.index()));
    }
  }, node);
}

Position get_pos_start(const NodeVariant& node) {
  return std::visit([&](const auto& val) -> Position {
    if constexpr (std::is_same_v<std::decay_t<decltype(val)>, NumberNode>) {
      return val.pos_start;
    } else if constexpr (
      std::is_same_v<decltype(val), SharedBin> ||
      std::is_same_v<decltype(val), SharedUnary>
    ) {
      return val->pos_start.value();
    } else {
      throw std::runtime_error("get_pos_start: unhandled node type:" + std::to_string(node.index()));
    }
  }, node);
}