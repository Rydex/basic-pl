#ifndef _SYMBOL_TABLE
#define _SYMBOL_TABLE


#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include "../token.h"

class SymbolTable {
private:
  std::shared_ptr<SymbolTable> parent = nullptr;
  std::unordered_map<std::string, TokenValue> symbols{};
public:
  std::optional<TokenValue> get(const std::string& name) const;

  void remove(const std::string& name);

  void set(const std::string& name, const TokenValue& value);
};

#endif