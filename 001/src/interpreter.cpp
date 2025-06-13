#include "interpreter.h"
#include "parser.h"
#include "position.h"
#include "lexer.h"
#include <functional>
#include <stdexcept>

Number::Number(std::optional<int> value): value(value) {
	set_pos();
}

Number& Number::set_pos(
	const std::optional<Position>& pos_start,
	const std::optional<Position>& pos_end
) {
	this->pos_start = pos_start;
	this->pos_end = pos_end;

	return *this;
}

Number Number::added_to(const Number& other) const {
	return Number(this->value.value() + other.value.value());
}

Number Number::subbed_by(const Number& other) const {
	return Number(this->value.value() - other.value.value());
}

Number Number::multiplied_by(const Number& other) const {
	return Number(this->value.value() * other.value.value());
}

Number Number::divided_by(const Number& other) const {
	return Number(this->value.value() / other.value.value());
}

std::string Number::as_string() const {
	return std::to_string(value.value());
}

Number Interpreter::visit(const NodeVariant& node) {
	std::function<Number(const NodeVariant& node)> method = [this](const NodeVariant& node) -> Number {
		if(std::holds_alternative<NumberNode>(node)) {
			Number num = visit_NumberNode(std::get<NumberNode>(node));
			return num;

		} else if(std::holds_alternative<SharedBin>(node)) {
			Number num = visit_BinOpNode(*std::get<SharedBin>(node));
			return num;

		} else if(std::holds_alternative<SharedUnary>(node)) {
			Number num = visit_UnaryOpNode(*std::get<SharedUnary>(node));
			return num;

		}

		throw std::runtime_error("no visit method defined");
	};

	return method(node);
}

Number Interpreter::visit_NumberNode(const NumberNode& node) {
	Token node_value = node.tok.value();
	TokenValue value = node_value.value.value();

	if(std::holds_alternative<int>(value)) {
		return Number(std::get<int>(value)).set_pos(node_value.pos_start.value());
	} else {
		return Number(std::get<double>(value)).set_pos(node_value.pos_start.value());
	}
}

Number Interpreter::visit_BinOpNode(const BinOpNode& node) {
	Number left = visit(node.left_node);
	Number right = visit(node.right_node);
	Number result(std::nullopt);

	if(node.op_tok.type == PLS_T) {
		result = left.added_to(right);
	} else if(node.op_tok.type == MIN_T) {
		result = left.subbed_by(right);
	} else if(node.op_tok.type == MUL_T) {
		result = left.multiplied_by(right);
	} else if(node.op_tok.type == DIV_T) {
		result = left.divided_by(right);
	}

	return result.set_pos(node.pos_start, node.pos_end);
}

Number Interpreter::visit_UnaryOpNode(const UnaryOpNode& node) {
	Number number = visit(node.node);

	if(node.op_tok.type == MIN_T) {
		number = number.multiplied_by(Number(-1));
	}

	return number.set_pos(node.pos_start, node.pos_end);
}