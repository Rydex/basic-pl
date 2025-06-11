#ifndef TOKEN
#define TOKEN

#include <variant>
#include <optional>
#include <string>
#include <sstream>
#include "position.h"

using TokenValue = std::variant<int, double>;

struct Token {
  std::string type;
  std::optional<TokenValue> value;
  std::optional<Position> pos_start, pos_end;

  Token(
    const std::string& type,
    const std::optional<TokenValue>& value = std::nullopt,
    const std::optional<Position>& pos_start = std::nullopt,
    const std::optional<Position>& pos_end = std::nullopt
  );

  std::string as_string() const;
};

#endif