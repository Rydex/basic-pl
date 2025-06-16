#include "interpreter.h"
#include "exception.h"
#include "parser.h"
#include "position.h"
#include "lexer.h"
#include <sstream>
#include <stdexcept>
#include <cmath>

Number RTResult::register_(const RTResult& res) {
  if(res.error) this->error = res.error;
  return res.value.value();
}

RTResult& RTResult::success(const Number& value) {
  this->value = value;
  return *this;
}

RTResult& RTResult::failure(const std::shared_ptr<Exception>& error) {
  this->error = std::move(error);
  this->value = std::nullopt;
  return *this;
}

Number::Number(double value): value(value) {
  set_pos();
  set_context();
}

Number& Number::set_pos(
  const std::optional<Position>& pos_start,
  const std::optional<Position>& pos_end
) {
  this->pos_start = pos_start;
  this->pos_end = pos_end;

  return *this;
}

Number& Number::set_context(const std::optional<Context>& context) {
  this->context = context;
  return *this;
}

NumberPair Number::added_to(const Number& other) const {
  return { 
    Number(this->value + other.value).set_context(this->context), nullptr };
}

NumberPair Number::subbed_by(const Number& other) const {
  return { Number(this->value - other.value).set_context(this->context), nullptr };
}

NumberPair Number::multiplied_by(const Number& other) const {
  return { Number(this->value * other.value).set_context(this->context), nullptr };
}

NumberPair Number::divided_by(const Number& other) const {
  if(other.value == 0) {
    return { std::nullopt, std::make_shared<RTException>(RTException(
      other.context,
      other.pos_start.value(), other.pos_end.value(),
      "division by zero"
      )) };
  }

  return { Number(this->value / other.value).set_context(this->context), nullptr };
}

NumberPair Number::powed_by(const Number& other) const {
  return { Number(std::pow(this->value, other.value)).set_context(this->context), nullptr };
}

NumberPair Number::modded_by(const Number& other) const {
  if(other.value == 0) {
    return { std::nullopt, std::make_shared<RTException>(RTException(
      other.context,
      other.pos_start.value(), other.pos_end.value(),
      "modulus by zero"
    )) };
  }

  return { Number(std::fmod(this->value, other.value)).set_context(this->context), nullptr };
}

std::string Number::as_string() const {
  std::ostringstream oss;
  oss << value;
  return oss.str();
}

RTResult Interpreter::visit(const NodeVariant& node, const Context& context) {
  return std::visit([&](const auto& val) -> RTResult {
    using T = std::decay_t<decltype(val)>;

    if constexpr (std::is_same_v<T, NumberNode>) {
      return visit_NumberNode(val, context);
    } else if constexpr (std::is_same_v<T, SharedBin>) {
      return visit_BinOpNode(*val, context);
    } else if constexpr (std::is_same_v<T, SharedUnary>) {
      return visit_UnaryOpNode(*val, context);
    }

    throw std::runtime_error("no visit method defined");
  }, node);
}

RTResult Interpreter::visit_NumberNode(const NumberNode& node, const Context& context) {
  Token node_value = node.tok.value();
  TokenValue value = node_value.value.value();

  if(std::holds_alternative<int>(value)) {
    return RTResult().success(
      Number(std::get<int>(value)).set_context(context).set_pos(
        node_value.pos_start.value(),
        node_value.pos_end.value()
      )
    );
  } else {
    return RTResult().success(
      Number(std::get<double>(value)).set_context(context).set_pos(
        node_value.pos_start.value(),
        node_value.pos_end.value()
      )
    );
  }
}

RTResult Interpreter::visit_BinOpNode(const BinOpNode& node, const Context& context) {
  RTResult res;
  Number left = res.register_(visit(node.left_node, context));
  if(res.error) return res;

  Number right = res.register_(visit(node.right_node, context));
  if(res.error) return res;

  std::optional<Number> result;
  std::shared_ptr<Exception> error;

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

  } else if(node.op_tok.type == POW_T) {
    const auto&[res, err] = left.powed_by(right);
    result = res;
    error = err;

  } else if(node.op_tok.type == MOD_T) {
    const auto&[res,err] = left.modded_by(right);
    result = res;
    error = err;

  }


  if(error && !result.has_value())
    return res.failure(error);

  Number result_pos = result.value();
  result_pos.set_pos(node.pos_start.value(), node.pos_end.value());
  return res.success(result_pos);
}

RTResult Interpreter::visit_UnaryOpNode(const UnaryOpNode& node, const Context& context) {
  RTResult res;
  Number number = res.register_(visit(node.node, context));
  if(res.error) return res;

  std::shared_ptr<Exception> err;

  if(node.op_tok.type == MIN_T) {
    const auto&[result, error] = number.multiplied_by(Number(-1));
  }

  if(err) return res.failure(err);
  
  return res.success(number.set_pos(node.pos_start, node.pos_end));
}