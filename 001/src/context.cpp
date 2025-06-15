#include "context.h"

// start context

Context::Context(
  const std::string& display_name,
  const std::optional<std::shared_ptr<Context>>& parent,
  const std::optional<Position>& parent_entry_pos
): display_name(display_name), parent(parent), parent_entry_pos(parent_entry_pos) {}

// end context