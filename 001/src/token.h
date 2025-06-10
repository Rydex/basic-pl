#ifndef TOKEN
#define TOKEN

#include <variant>
#include <optional>
#include <string>
#include <sstream>

using TokenValue = std::variant<int, double>;

struct Token {
	std::string type;
	std::optional<TokenValue> value;

	Token(const std::string& type, const std::optional<TokenValue>& value = std::nullopt);

	std::string as_string() const;
};

#endif