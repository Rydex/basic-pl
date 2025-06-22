#include "token.h"
#include <string>
#include <type_traits>

Token::Token(
  const std::string& type,
  const std::optional<TokenValue>& value,
  const std::optional<Position>& pos_start,
  const std::optional<Position>& pos_end
): type(type), value(value) {
  if(pos_start) {
    this->pos_start = pos_start->copy();
    this->pos_end = pos_start->copy();
    this->pos_end->advance();
  }

  if(pos_end) {
    this->pos_end = pos_end;
  }
}

bool Token::matches(const std::string& type, const TokenValue& val) const {
  // return this->type == type && this->value.value() == value;
  if(!this->value) return false;

  return std::visit([&](const auto& lhs, const auto& rhs) -> bool {
    using L = std::decay_t<decltype(lhs)>;
    using R = std::decay_t<decltype(rhs)>;

    if constexpr (std::is_same_v<L, R>) {
      return lhs == rhs;
    } else {
      return false;
    }
  }, this->value.value(), val);
}