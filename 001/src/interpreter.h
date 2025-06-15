#ifndef INTERPRETER
#define INTERPRETER

#include "parser.h"
#include "position.h"
#include <functional>

class Number;

using NumberPair = std::pair<
	std::optional<Number>,
	std::optional<Exception>
>;

class Number {
protected:
	double value;
	std::optional<Position> pos_start, pos_end;

public:
	Number(double value);

	Number& set_pos(
		const std::optional<Position>& pos_start = std::nullopt,
		const std::optional<Position>& pos_end = std::nullopt
	);

	NumberPair added_to(const Number& other) const;
	NumberPair subbed_by(const Number& other) const;
	NumberPair multiplied_by(const Number& other) const;
	NumberPair divided_by (const Number& other) const;

	std::string as_string() const;
};

class RTResult {
public:
	std::optional<Number> value = std::nullopt;
	std::optional<Exception> error = std::nullopt;

	Number register_(const RTResult& res);
	RTResult& success(const Number& value);
	RTResult& failure(const Exception& error);
};

class Interpreter {
public:
	RTResult visit(const NodeVariant& node);
	RTResult visit_NumberNode(const NumberNode& node);
	RTResult visit_BinOpNode(const BinOpNode& node);
	RTResult visit_UnaryOpNode(const UnaryOpNode& node);
};


#endif