#include "token.h"

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
  if(!value) {
    return type;
  } else if(std::holds_alternative<int>(value.value())) {
    return type + ':' + std::to_string(std::get<int>(value.value()));
  } else {
    std::ostringstream oss;
    oss << std::get<double>(value.value());
    return type + ':' + oss.str();
  }
}