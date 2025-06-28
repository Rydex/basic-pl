#include "values.h"

Value::Value() {
	set_pos();
}

Value& Value::set_context(
	const std::optional<Context>& context
) {
	this->context = context;

	return *this;
}

Value& Value::set_pos(
	const std::optional<Position>& pos_start,
  const std::optional<Position>& pos_end
) {
	this->pos_start = pos_start;
	this->pos_end = pos_end;

	return *this;
}