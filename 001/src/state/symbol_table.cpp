#include "symbol_table.h"

std::optional<TokenValue> SymbolTable::get(const std::string& name) const {
	std::optional<TokenValue> value = std::nullopt;

	decltype(symbols)::const_iterator it = symbols.find(name);

	if(it != symbols.end()) {
		value = it->second;
	} else {
		value = std::nullopt;
	}

	if(!value && parent) {
		return parent->get(name);
	}

	return value;
}

void SymbolTable::remove(const std::string& name) {
	symbols.erase(name);
}

void SymbolTable::set(const std::string& name, const TokenValue& value) {
	std::visit([&](const auto& val) -> void {
		symbols[name] = val;
	}, value);
}