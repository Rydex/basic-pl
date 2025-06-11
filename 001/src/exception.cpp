#include "exception.h"

Exception::Exception(
  const Position& pos_start,
  const Position& pos_end,
  const std::string& message,
  const std::string& details
)
  : pos_start(pos_start), pos_end(pos_end), message(message), details(details) {}

std::string Exception::as_string() const {
  std::string result = message + ": " + details;
  result += "\nFile " + pos_start.get_fn() + ", line " + std::to_string(pos_start.get_ln() + 1);
  return result;
}

IllegalCharException::IllegalCharException(
  const Position& pos_start,
  const Position& pos_end,
  char ch
)
  : Exception(pos_start, pos_end, "Illegal Character", "'" + std::string(1, ch) + "'") {}

InvalidSyntaxException::InvalidSyntaxException(
  const Position& pos_start,
  const Position& pos_end,
  const std::string& details
)
  : Exception(pos_start, pos_end, "Invalid Syntax", details) {}