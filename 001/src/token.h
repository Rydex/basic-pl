#ifndef TOKEN
#define TOKEN

#include <memory>
#include <variant>
#include <optional>
#include <string>
#include <sstream>
#include "position.h"

class Number;

using TokenValue = std::variant<int, double, std::string, std::shared_ptr<Number>>;

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

  bool matches(const std::string& type, const TokenValue& value) const;
};

#endif