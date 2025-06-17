#include "symbol_table.h"
#include <optional>

std::optional<TokenValue> SymbolTable::get(const std::string& name) const {
  std::optional<TokenValue> value = std::nullopt;

  try {
    value = symbols.at(name);
  } catch (const std::out_of_range&) {
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
  std::visit([&](const auto& val) -> void { symbols[name] = val; }, value);

  //symbols[name] = value;
}