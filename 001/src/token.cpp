#include "token.h"
#include <string>

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

std::string Token::as_string() const {
  if(!value)
    return type;
  // } else if(std::holds_alternative<int>(value.value())) {
  //   return type + ':' + std::to_string(std::get<int>(value.value()));
  // } else if(std::holds_alternative<double>(value.value())) {
  //   std::ostringstream oss;
  //   oss << std::get<double>(value.value());
  //   return type + ':' + oss.str();
  // }

  return std::visit([this](auto&& val) -> std::string {
    if constexpr (std::is_same_v<std::decay_t<decltype(val)>, int>) {
      return type + ':' + std::to_string(val);
    } else if constexpr (std::is_same_v<std::decay_t<decltype(val)>, double>) {
      std::ostringstream oss;
      oss << val;
      return type + ':' + oss.str();
    } else {
      return type + ':' + val;
    }
  }, value.value());
}