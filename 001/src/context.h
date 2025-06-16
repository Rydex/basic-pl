#ifndef CONTEXT
#define CONTEXT

#include "position.h"
#include "state/symbol_table.h"
#include <memory>
#include <optional>
#include <string>

class Context {
public:
	std::string display_name;
	std::optional<std::shared_ptr<Context>> parent;
	std::optional<Position> parent_entry_pos;
	std::optional<SymbolTable> symbol_table = std::nullopt;

	Context(
		const std::string& display_name,
		const std::optional<std::shared_ptr<Context>>& parent = std::nullopt,
		const std::optional<Position>& parent_entry_pos = std::nullopt
	);
};

#endif