#include "token.h"

Token::Token(const std::string& type, const std::optional<TokenValue>& value)
  : type(type), value(value) {}

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