#ifndef INTERPRETER
#define INTERPRETER

#include "parser.h"
#include "position.h"
#include <functional>

class Number {
protected:
	std::optional<int> value;
	std::optional<Position> pos_start, pos_end;

public:
	Number(std::optional<int> value);

	Number& set_pos(
		const std::optional<Position>& pos_start = std::nullopt,
		const std::optional<Position>& pos_end = std::nullopt
	);

	Number added_to(const Number& other) const;
	Number subbed_by(const Number& other) const;
	Number multiplied_by(const Number& other) const;
	Number divided_by (const Number& other) const;

	std::string as_string() const;
};

class Interpreter {
public:
	Number visit(const NodeVariant& node);
	Number visit_NumberNode(const NumberNode& node);
	Number visit_BinOpNode(const BinOpNode& node);
	Number visit_UnaryOpNode(const UnaryOpNode& node);
};

#endif