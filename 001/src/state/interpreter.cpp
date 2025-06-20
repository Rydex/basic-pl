#include "interpreter.h"
#include "../exception.h"
#include "../position.h"
#include "../lexer.h"
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <cmath>

Number RTResult::register_(const RTResult& res) {
  if(res.error) this->error = res.error;
  if(res.value)
    //return res.value.value();
    return std::visit([&](const auto& val) -> Number {
      if constexpr (std::is_same_v<std::decay_t<decltype(val)>, Number>) {
        return val;
      } else {
        throw std::runtime_error("unsupported in register_()");
      }
    }, res.value.value());

  return Number(-1);
}

RTResult& RTResult::success(const RTVariant& value) {
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

Number Number::copy() {
  return Number(value)
    .set_pos(pos_start.value(), pos_end.value())
    .set_context(context);
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
    return { std::nullopt, std::make_shared<RTException>(
      other.context,
      other.pos_start.value(), other.pos_end.value(),
      "division by zero"
      ) };
  }

  return { Number(this->value / other.value).set_context(this->context), nullptr };
}

NumberPair Number::powed_by(const Number& other) const {
  return { 
    Number(std::pow(this->value, other.value)).set_context(this->context),
    nullptr 
  };
}

NumberPair Number::modded_by(const Number& other) const {
  if(other.value == 0) {
    return { std::nullopt, std::make_shared<RTException>(
      other.context,
      other.pos_start.value(), other.pos_end.value(),
      "modulus by zero"
    ) };
  }

  return { 
    Number(std::fmod(this->value, other.value)).set_context(this->context), 
    nullptr
  };
}

std::string Number::as_string() const {
  std::ostringstream oss;
  oss << value;
  return oss.str();
}

RTResult Interpreter::visit(const std::shared_ptr<ASTNode>& node, Context& context) const {
  return node->accept(*this, context);
}

RTResult Interpreter::visit_VarAccessNode(const VarAccessNode& node, Context& context) const {
  RTResult res;
  std::string var_name = std::get<std::string>(node.var_name_tok.value.value());
  std::optional<TokenValue> value;

  try {
    value = context.symbol_table->get(var_name).value();
  } catch (std::bad_optional_access&) {
    value = std::nullopt;
  }

  if(!value) {
    return res.failure(std::make_shared<RTException>(
      context,
      node.pos_start, node.pos_end,
      var_name + " is not defined"
    ));
  }

  return std::visit([&](const auto& val) -> RTResult {
    using T = std::decay_t<decltype(val)>;

    if constexpr (std::is_same_v<T, int>) {
      return res.success(
        Number(static_cast<double>(val))
        .set_context(context)
        .set_pos(node.pos_start, node.pos_end)
      );
    } else if constexpr (std::is_same_v<T, double>) {
      return res.success(
        Number(val)
        .set_context(context)
        .set_pos(node.pos_start, node.pos_end)
      );
    } else {
      throw std::runtime_error("visit_VarAccessNode");
    }
  }, value.value());
}

RTResult Interpreter::visit_VarAssignNode(const VarAssignNode& node, Context& context) const {
  RTResult res;

  TokenValue tok_val = node.var_name_tok.value.value();
  std::string var_name = std::get<std::string>(tok_val);

  RTResult value_expr = visit(node.value_node, context);
  Number value = res.register_(value_expr);

  if(res.error) return res;

  if(var_name == "null" || var_name == "quit") {
    return res.failure(std::make_shared<RTException>(
      context,
      node.pos_start, node.pos_end,
      "cannot reassign built-in variable " + var_name
    ));
  }

  // std::cout << "setting " << var_name << " to value " << value.get_value();

  context.symbol_table->set(var_name, std::get<double>(value.get_value()));
  return res.success(value);
}

RTResult Interpreter::visit_NumberNode(const NumberNode& node, Context& context) const {
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

RTResult Interpreter::visit_BinOpNode(const BinOpNode& node, Context& context) const {
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

  } else if(node.op_tok.type == EE_T) {
    const auto&[res, err] = left.eq_comp(right);
    result = res;
    error = err;

  } else if(node.op_tok.type == NE_T) {
    const auto&[res, err] = left.ne_comp(right);
    result = res;
    error = err;

  } else if(node.op_tok.type == LT_T) {
    const auto&[res, err] = left.lt_comp(right);
    result = res;
    error = err;

  } else if(node.op_tok.type == GT_T) {
    const auto&[res, err] = left.gt_comp(right);
    result = res;
    error = err;

  } else if(node.op_tok.type == LTE_T) {
    const auto&[res, err] = left.lte_comp(right);
    result = res;
    error = err;

  } else if(node.op_tok.type == GTE_T) {
    const auto&[res, err] = left.gte_comp(right);
    result = res;
    error = err;

  } else if(node.op_tok.matches()) {
    
  }

  if(error && !result.has_value())
    return res.failure(error);

  Number result_pos = result.value();
  result_pos.set_pos(node.pos_start.value(), node.pos_end.value());
  return res.success(result_pos);
}

RTResult Interpreter::visit_UnaryOpNode(const UnaryOpNode& node, Context& context) const {
  RTResult res;
  Number number = res.register_(visit(node.node, context));
  if(res.error) return res;

  std::shared_ptr<Exception> err;

  if(node.op_tok.type == MIN_T) {
    const auto&[result, error] = number.multiplied_by(Number(-1));

    number = result.value();
    err = error;
  }

  if(err) return res.failure(err);
  
  return res.success(number.set_pos(node.pos_start, node.pos_end));
}