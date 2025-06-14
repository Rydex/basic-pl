	#include "interpreter.h"
#include <cassert>
	#include "parser.h"
	#include "position.h"
	#include "lexer.h"
	#include <sstream>
	#include <stdexcept>

	// run time result

	RTVariant RTResult::register_(const RTResult& res) {
		// if(res.error) this->error = res.error.value();
		// return res.value.value();

		// std::visit([this](const auto& val) {
		// 	if(val.error) this->error = val.error;
		// }, res);

		if(res.error) this->error = res.error;
		return res.value.value();
	}

	RTResult& RTResult::success(const Number& value) {
		this->value = value;
		return *this;
	}

	RTResult& RTResult::failure(const Exception& error) {
		this->error = error;
		return *this;
	}

	// end rt result

	Number::Number(double value): value(value) {
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

	NumberPair Number::added_to(const Number& other) const {
		return { Number(this->value + other.value), std::nullopt };
	}

	NumberPair Number::subbed_by(const Number& other) const {
		return { Number(this->value - other.value), std::nullopt };
	}

	NumberPair Number::multiplied_by(const Number& other) const {
		return { Number(this->value * other.value), std::nullopt };
	}

	NumberPair Number::divided_by(const Number& other) const {
		if(other.value == 0)
			return { std::nullopt, RTException(
				other.pos_start.value(), other.pos_end.value(),
				"division by zero"
			) };

		return { Number(this->value / other.value), std::nullopt };
	}

	std::string Number::as_string() const {
		std::ostringstream oss;
		oss << value;
		return oss.str();
	}

	RTResult Interpreter::visit(const NodeVariant& node) {
		return std::visit([this](const auto& val) -> RTResult {
			using T = std::decay_t<decltype(val)>;

			if constexpr (std::is_same_v<T, NumberNode>) {
				return visit_NumberNode(val);
			} else if constexpr (std::is_same_v<T, SharedBin>) {
				return visit_BinOpNode(*val);
			} else if constexpr (std::is_same_v<T, SharedUnary>) {
				return visit_UnaryOpNode(*val);
			}

			throw std::runtime_error("no visit method defined");
		}, node);

	}

	RTResult Interpreter::visit_NumberNode(const NumberNode& node) {
		Token node_value = node.tok.value();
		TokenValue value = node_value.value.value();

		if(std::holds_alternative<int>(value)) {
			return RTResult().success( 
				Number(std::get<int>(value)).set_pos(node_value.pos_start.value())
			);
		} else {
			return RTResult().success(
				Number(std::get<double>(value)).set_pos(node_value.pos_start.value())
			);
		}
	}

	RTResult Interpreter::visit_BinOpNode(const BinOpNode& node) {
		RTResult res;
		// RTResult left_res = res.register_(visit(node.left_node));
		RTResult left_res = visit(node.left_node);
		RTVariant left_val = res.register_(left_res);

		if(left_res.error) return left_res;

		Number left = left_res.value.value();
		
		RTResult right_res = visit(node.right_node);
		if(right_res.error) return right_res;

		Number right = right_res.value.value();

		std::optional<Number> result;

		std::optional<Exception> error;

		if(node.op_tok.type == PLS_T) {
			const auto&[res, err] = left.added_to(right);
			result = res;
			error = err;

		} else if(node.op_tok.type == MIN_T) {
			const auto&[res, err] = left.subbed_by(right);
			result = res;
			error = err;

		} else if(node.op_tok.type == MUL_T) {
			const auto&[res, err] = left.multiplied_by(right);
			result = res;
			error = err;

		} else if(node.op_tok.type == DIV_T) {
			const auto&[res, err] = left.divided_by(right);
			result = res;
			error = err;

		}

		if(error) return res.failure(error.value());

		return res.success(
			result->set_pos(node.pos_start, node.pos_end)
		);
	}

	RTResult Interpreter::visit_UnaryOpNode(const UnaryOpNode& node) {
		RTResult res;
		RTResult number_res = visit(node.node);

		if(number_res.error) return number_res;

		Number number = number_res.value.value();
		std::optional<Exception> error = std::nullopt;

		if(node.op_tok.type == MIN_T) {
			const auto&[result, err] = number.multiplied_by(Number(-1));
			error = err;
		}

		if(error) return res.failure(error.value());

		return res.success(
			number.set_pos(node.pos_start, node.pos_end)
		);
	}