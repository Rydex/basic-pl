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