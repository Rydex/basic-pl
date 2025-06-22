#include "interpreter.h"
#include "../exception.h"
#include "../position.h"
#include "../lexer.h"
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <cmath>

std::shared_ptr<Value> RTResult::register_(const RTResult& res) {
  if(res.error) this->error = res.error;
  if(res.value)
    //return res.value.value();
    return std::make_shared<Value>(std::visit([&](const auto& val) -> Number {
        if constexpr (std::is_same_v<std::decay_t<decltype(val)>, Number>) {
          return std::make_shared<Number>(val);
        } else {
          throw std::runtime_error("unsupported in register_()");
        }
      }, res.value.value())
    );

  return std::make_shared<Number>(-1);
}

RTResult& RTResult::success(const std::optional<RTVariant>& value) {
  this->value = value;
  return *this;
}

RTResult& RTResult::failure(const std::shared_ptr<Exception>& error) {
  this->error = std::move(error);
  this->value = std::nullopt;
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

Value& Value::set_context(
  const std::optional<Context>& context
) {
  this->context = context;
  return *this;
}

ValuePair Value::added_to(const std::shared_ptr<Value>& other) const {
  return { nullptr, illegal_operation().error };
}

ValuePair Value::subbed_by(const std::shared_ptr<Value>& other) const {
  return { nullptr, illegal_operation().error };
}

ValuePair Value::multiplied_by(const std::shared_ptr<Value>& other) const {
  return { nullptr, illegal_operation().error };
}

ValuePair Value::divided_by(const std::shared_ptr<Value>& other) const {
  return { nullptr, illegal_operation().error };
}

ValuePair Value::powed_by(const std::shared_ptr<Value>& other) const {
  return { nullptr, illegal_operation().error };
}

ValuePair Value::modded_by(const std::shared_ptr<Value>& other) const {
  return { nullptr, illegal_operation().error };
}

ValuePair Value::eq_comp(const std::shared_ptr<Value>& other) const {
  return { nullptr, illegal_operation().error };
}

ValuePair Value::ne_comp(const std::shared_ptr<Value>& other) const {
  return { nullptr, illegal_operation().error };
}

ValuePair Value::lt_comp(const std::shared_ptr<Value>& other) const {
  return { nullptr, illegal_operation().error };
}

ValuePair Value::gt_comp(const std::shared_ptr<Value>& other) const {
  return { nullptr, illegal_operation().error };
}

ValuePair Value::lte_comp(const std::shared_ptr<Value>& other) const {
  return { nullptr, illegal_operation().error };
}

ValuePair Value::gte_comp(const std::shared_ptr<Value>& other) const {
  return { nullptr, illegal_operation().error };
}

ValuePair Value::or_comp(const std::shared_ptr<Value>& other) const {
  return { nullptr, illegal_operation().error };
}

ValuePair Value::and_comp(const std::shared_ptr<Value>& other) const {
  return { nullptr, illegal_operation().error };
}

ValuePair Value::not_operator() const {
  return { nullptr, illegal_operation().error };
}

RTResult Value::execute(const std::vector<std::shared_ptr<Value>>& args) {
  return illegal_operation();
}

RTResult Value::illegal_operation(
  const std::optional<std::shared_ptr<Value>>& other
) const {
  return RTResult().failure(
    std::make_shared<RTException>(
      context.value(),
      pos_start.value(), pos_end.value(),
      "illegal operation"
    )
  );
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

std::shared_ptr<Value> Number::copy() {
  std::shared_ptr<Value> number = std::make_shared<Number>(value);
  number->set_pos(pos_start.value(), pos_end.value());
  number->set_context(context);
  return number;
}

ValuePair Number::added_to(const std::shared_ptr<Value>& other) const {
  auto other_type = std::dynamic_pointer_cast<Number>(other);

  if(!other_type)
    return { nullptr, illegal_operation().error };

  std::shared_ptr<Value> number = std::make_shared<Number>(this->value + other_type->value);
  number->set_context(this->context);
  return { number, nullptr };
}

ValuePair Number::subbed_by(const std::shared_ptr<Value>& other) const {
  auto other_type = std::dynamic_pointer_cast<Number>(other);

  if(!other_type)
    return { nullptr, illegal_operation().error };

  std::shared_ptr<Value> number = std::make_shared<Number>(this->value - other_type->value);
  number->set_context(this->context);
  return { number, nullptr };
}

ValuePair Number::multiplied_by(const std::shared_ptr<Value>& other) const {
  auto other_type = std::dynamic_pointer_cast<Number>(other);

  if(!other_type)
    return { nullptr, illegal_operation().error };

  std::shared_ptr<Value> number = std::make_shared<Number>(this->value * other_type->value);
  number->set_context(this->context);
  return { number, nullptr };
}

ValuePair Number::divided_by(const std::shared_ptr<Value>& other) const {
  auto other_type = std::dynamic_pointer_cast<Number>(other);

  if(!other_type)
    return { nullptr, illegal_operation().error };

  if(other_type->value == 0) {
    return { nullptr, std::make_shared<RTException>(
      other_type->context,
      other_type->pos_start.value(), other_type->pos_end.value(),
      "division by zero"
      ) };
  }

  std::shared_ptr<Value> number = std::make_shared<Number>(this->value / other_type->value);
  number->set_context(this->context);
  return { number, nullptr };
}

ValuePair Number::powed_by(const std::shared_ptr<Value>& other) const {
  auto other_type = std::dynamic_pointer_cast<Number>(other);

  if(!other_type)
    return { nullptr, illegal_operation().error };

  std::shared_ptr<Value> number = std::make_shared<Number>(std::pow(this->value, other_type->value));
  number->set_context(this->context);
  return { number, nullptr };
}

ValuePair Number::modded_by(const std::shared_ptr<Value>& other) const {
  auto other_type = std::dynamic_pointer_cast<Number>(other);

  if(!other_type)
    return { nullptr, illegal_operation().error };

  if(other_type->value == 0) {
    return { nullptr, std::make_shared<RTException>(
      other_type->context,
      other_type->pos_start.value(), other_type->pos_end.value(),
      "modulus by zero"
    ) };
  }

  std::shared_ptr<Value> number = std::make_shared<Number>(std::fmod(this->value, other_type->value));
  number->set_context(this->context);
  return { number, nullptr };
}

ValuePair Number::eq_comp(const std::shared_ptr<Value>& other) const {
  auto other_type = std::dynamic_pointer_cast<Number>(other);

  if(!other_type)
    return { nullptr, illegal_operation().error };

  std::shared_ptr<Value> number = std::make_shared<Number>(
    static_cast<int>(value == other_type->value)
  );
  number->set_context(context);
  return { number, nullptr };
}

ValuePair Number::ne_comp(const std::shared_ptr<Value>& other) const {
  auto other_type = std::dynamic_pointer_cast<Number>(other);

  if(!other_type)
    return { nullptr, illegal_operation().error };

  std::shared_ptr<Value> number = std::make_shared<Number>(
    static_cast<int>(value != other_type->value)
  );
  number->set_context(context);
  return { number, nullptr };
}

ValuePair Number::lt_comp(const std::shared_ptr<Value>& other) const {
  auto other_type = std::dynamic_pointer_cast<Number>(other);

  if(!other_type)
    return { nullptr, illegal_operation().error };

  std::shared_ptr<Value> number = std::make_shared<Number>(
    static_cast<int>(value < other_type->value)
  );
  number->set_context(context);
  return { number, nullptr };
}

ValuePair Number::gt_comp(const std::shared_ptr<Value>& other) const {
  auto other_type = std::dynamic_pointer_cast<Number>(other);

  if(!other_type)
    return { nullptr, illegal_operation().error };

  std::shared_ptr<Value> number = std::make_shared<Number>(
    static_cast<int>(value > other_type->value)
  );
  number->set_context(context);
  return { number, nullptr };
}

ValuePair Number::lte_comp(const std::shared_ptr<Value>& other) const {
  auto other_type = std::dynamic_pointer_cast<Number>(other);

  if(!other_type)
    return { nullptr, illegal_operation().error };

  std::shared_ptr<Value> number = std::make_shared<Number>(
    static_cast<int>(value <= other_type->value)
  );
  number->set_context(context);
  return { number, nullptr };
}

ValuePair Number::gte_comp(const std::shared_ptr<Value>& other) const {
  auto other_type = std::dynamic_pointer_cast<Number>(other);

  if(!other_type)
    return { nullptr, illegal_operation().error };

  std::shared_ptr<Value> number = std::make_shared<Number>(
    static_cast<int>(value >= other_type->value)
  );
  number->set_context(context);
  return { number, nullptr };
}

ValuePair Number::and_comp(const std::shared_ptr<Value>& other) const {
  auto other_type = std::dynamic_pointer_cast<Number>(other);

  if(!other_type)
    return { nullptr, illegal_operation().error };

  std::shared_ptr<Value> number = std::make_shared<Number>(
    static_cast<int>(value && other_type->value)
  );
  number->set_context(context);
  return { number, nullptr };
}

ValuePair Number::or_comp(const std::shared_ptr<Value>& other) const {
  auto other_type = std::dynamic_pointer_cast<Number>(other);

  if(!other_type)
    return { nullptr, illegal_operation().error };

  std::shared_ptr<Value> number = std::make_shared<Number>(
    static_cast<int>(value || other_type->value)
  );
  number->set_context(context);
  return { number, nullptr };
}

ValuePair Number::not_operator() const {
  std::shared_ptr<Value> number = std::make_shared<Number>(
    (value == 0) ? 1 : 0
  );
  number->set_context(context);
  return { number, nullptr };
}

RTResult Function::execute(const std::vector<std::shared_ptr<Value>>& args) {
  RTResult res;
  Interpreter interpreter;

  Context new_context(
    name,
    std::make_shared<Context>(this->context.value()),
    pos_start.value()
  );
  new_context.symbol_table = std::make_shared<SymbolTable>(
    new_context.parent.value()->symbol_table
  );

  if(static_cast<int>(args.size()) > static_cast<int>(arg_names.size())) {
    return res.failure(std::make_shared<RTException>(
      context,
      pos_start.value(), pos_end.value(),
      std::to_string(static_cast<int>(args.size()) - static_cast<int>(arg_names.size()))
      + " too many arguments passed into " + name
    ));
  }

  if(static_cast<int>(args.size()) < static_cast<int>(arg_names.size())) {
    return res.failure(std::make_shared<RTException>(
      context,
      pos_start.value(), pos_end.value(),
      std::to_string(static_cast<int>(args.size()) - static_cast<int>(arg_names.size()))
      + " too few arguments passed into " + name
    ));
  }

  for(int i=0; i<static_cast<int>(args.size()); i++) {
    std::string arg_name = arg_names.at(i);
    std::shared_ptr<Value> arg_value = args.at(i);
    arg_value->set_context(new_context);
    new_context.symbol_table->set(arg_name, arg_value);
  }

  std::shared_ptr<Value> value = res.register_(interpreter.visit(body, new_context));
}

bool Number::is_true() const {
  return value != 0;
}

std::string Number::as_string() const {
  std::ostringstream oss;
  oss << value;
  return oss.str();
}

// visit methods

RTResult Interpreter::visit(const std::shared_ptr<ASTNode>& node, Context& context) const {
  return node->accept(*this, context);
}

RTResult Interpreter::visit_VarAccessNode(const VarAccessNode& node, Context& context) const {
  RTResult res;
  std::string var_name = std::get<std::string>(node.var_name_tok.value.value());
  std::optional<TokenValue> value;

  try {
    value = context.symbol_table->get(var_name);
  } catch (std::out_of_range&) {
    value = std::nullopt;
  }

  if(!value) {
    return res.failure(std::make_shared<RTException>(
      context,
      node.pos_start, node.pos_end,
      "'" + var_name + "' is not defined"
    ));
  }

  return std::visit([&](const auto& val) -> RTResult {
    using T = std::decay_t<decltype(val)>;

    if constexpr (std::is_same_v<T, int>) {
      auto number = Number(static_cast<double>(val));
      number.set_context(context);
      number.set_pos(node.pos_start, node.pos_end);

      return res.success(std::make_shared<Number>(number));
    } else if constexpr (std::is_same_v<T, double>) {
      auto number = Number(val);
      number.set_context(context);
      number.set_pos(node.pos_start, node.pos_end);

      return res.success(std::make_shared<Number>(number));
    } else if constexpr (std::is_same_v<T, std::shared_ptr<Number>>) {
      auto number = Number(val->value);
      number.set_context(context);
      number.set_pos(node.pos_start, node.pos_end);

      return res.success(std::make_shared<Number>(number));
    } else if constexpr (std::is_same_v<T, std::shared_ptr<Function>>) {
      auto function = Function(val->name, val->body, val->arg_names);
      function.set_context(context);
      function.set_pos(node.pos_start, node.pos_end);

      return res.success(std::make_shared<Function>(function));
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
  std::shared_ptr<Value> value = res.register_(value_expr);


  if(res.error) return res;

  if(std::ranges::find(builtins, var_name) != builtins.end()) {
    return res.failure(std::make_shared<RTException>(
      context,
      node.pos_start, node.pos_end,
      "cannot reassign built-in variable '" + var_name + "'"
    ));
  }

  if(!value_expr.value) {
    return res.failure(std::make_shared<RTException>(
      context,
      node.pos_start, node.pos_end,
      "'" + var_name + "' is not defined"
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
    const auto&[res, err] = left.added_to(std::make_shared<Number>(right));

    auto res_type = std::dynamic_pointer_cast<Number>(res);

    if(!res_type)
      throw std::runtime_error("visit_BinOpNode, line 485");

    result = *res_type;
    error = err;

  } else if(node.op_tok.type == MIN_T) {
    const auto&[res, err] = left.subbed_by(std::make_shared<Number>(right));

    auto res_type = std::dynamic_pointer_cast<Number>(res);

    if(!res_type)
      throw std::runtime_error("visit_BinOpNode, line 496");

    result = *res_type;
    error = err;

  } else if(node.op_tok.type == MUL_T) {
    const auto&[res, err] = left.multiplied_by(std::make_shared<Number>(right));

    auto res_type = std::dynamic_pointer_cast<Number>(res);

    if(!res_type)
      throw std::runtime_error("visit_BinOpNode, line 507");

    result = *res_type;
    error = err;

  } else if(node.op_tok.type == DIV_T) {
    const auto&[res, err] = left.divided_by(std::make_shared<Number>(right));

    auto res_type = std::dynamic_pointer_cast<Number>(res);

    if(!res_type)
      throw std::runtime_error("visit_BinOpNode, line 518");

    result = *res_type;
    error = err;

  } else if(node.op_tok.type == POW_T) {
    const auto&[res, err] = left.powed_by(std::make_shared<Number>(right));

    auto res_type = std::dynamic_pointer_cast<Number>(res);

    if(!res_type)
      throw std::runtime_error("visit_BinOpNode, line 529");

    result = *res_type;
    error = err;

  } else if(node.op_tok.type == MOD_T) {
    const auto&[res, err] = left.modded_by(std::make_shared<Number>(right));

    auto res_type = std::dynamic_pointer_cast<Number>(res);

    if(!res_type)
      throw std::runtime_error("visit_BinOpNode, line 540");

    result = *res_type;
    error = err;

  } else if(node.op_tok.type == EE_T) {
    const auto&[res, err] = left.eq_comp(std::make_shared<Number>(right));

    auto res_type = std::dynamic_pointer_cast<Number>(res);

    if(!res_type)
      throw std::runtime_error("visit_BinOpNode, line 540");

    result = *res_type;
    error = err;

  } else if(node.op_tok.type == NE_T) {
    const auto&[res, err] = left.ne_comp(std::make_shared<Number>(right));

    auto res_type = std::dynamic_pointer_cast<Number>(res);

    if(!res_type)
      throw std::runtime_error("visit_BinOpNode, line 562");

    result = *res_type;
    error = err;

  } else if(node.op_tok.type == LT_T) {
    const auto&[res, err] = left.lt_comp(std::make_shared<Number>(right));

    auto res_type = std::dynamic_pointer_cast<Number>(res);

    if(!res_type)
      throw std::runtime_error("visit_BinOpNode, line 573");

    result = *res_type;
    error = err;

  } else if(node.op_tok.type == GT_T) {
    const auto&[res, err] = left.gt_comp(std::make_shared<Number>(right));

    auto res_type = std::dynamic_pointer_cast<Number>(res);

    if(!res_type)
      throw std::runtime_error("visit_BinOpNode, line 584");

    result = *res_type;
    error = err;

  } else if(node.op_tok.type == LTE_T) {
    const auto&[res, err] = left.lte_comp(std::make_shared<Number>(right));

    auto res_type = std::dynamic_pointer_cast<Number>(res);

    if(!res_type)
      throw std::runtime_error("visit_BinOpNode, line 595");

    result = *res_type;
    error = err;

  } else if(node.op_tok.type == GTE_T) {
    const auto&[res, err] = left.gte_comp(std::make_shared<Number>(right));

    auto res_type = std::dynamic_pointer_cast<Number>(res);

    if(!res_type)
      throw std::runtime_error("visit_BinOpNode, line 606");

    result = *res_type;
    error = err;

  } else if(node.op_tok.matches(KWD_T, "and")) {
    const auto&[res, err] = left.and_comp(std::make_shared<Number>(right));

    auto res_type = std::dynamic_pointer_cast<Number>(res);

    if(!res_type)
      throw std::runtime_error("visit_BinOpNode, line 617");

    result = *res_type;
    error = err;

  } else if(node.op_tok.matches(KWD_T, "or")) {
    const auto&[res, err] = left.or_comp(std::make_shared<Number>(right));

    auto res_type = std::dynamic_pointer_cast<Number>(res);

    if(!res_type)
      throw std::runtime_error("visit_BinOpNode, line 628");

    result = *res_type;
    error = err;

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
    const auto&[result, error] = number.multiplied_by(std::make_shared<Number>(-1));

    auto downcasted_ptr = std::dynamic_pointer_cast<Number>(result);

    if(!downcasted_ptr)
      throw std::runtime_error("visit_UnaryOpNode, line 657");

    number = downcasted_ptr->value;
    err = error;

  } else if(node.op_tok.matches(KWD_T, "not")) {
    const auto&[result, error] = number.not_operator();

    auto downcasted_ptr = std::dynamic_pointer_cast<Number>(result);

    if(!downcasted_ptr)
      throw std::runtime_error("visit_UnaryOpNode, line 663");

    number = downcasted_ptr->value;
    err = error;
  }

  if(err) return res.failure(err);
  
  return res.success(number.set_pos(node.pos_start, node.pos_end));
}

RTResult Interpreter::visit_IfNode(const IfNode& node, Context& context) const {
  RTResult res;

  for(const auto&[condition, expr] : node.cases) {
    Number condition_value = res.register_(visit(condition, context));
    if(res.error) return res;

    if(condition_value.is_true()) {
      Number expr_value = res.register_(visit(expr, context));
      if(res.error) return res;

      return res.success(expr_value);
    }
  }

  if(node.else_case) {
    Number else_value = res.register_(visit(node.else_case, context));
    if(res.error) return res;
    return res.success(else_value);
  }

  return res.success(std::nullopt);
}

RTResult Interpreter::visit_ForNode(const ForNode& node, Context& context) const {
  RTResult res;

  Number start_value = res.register_(visit(node.start_value, context));
  if(res.error) return res;

  Number end_value = res.register_(visit(node.end_value, context));
  if(res.error) return res;

  Number step_value(-1);

  if(node.step_value) {
    step_value = res.register_(visit(node.step_value, context));
    if(res.error) return res;
  } else {
    step_value = 1;
  }

  double i = std::get<double>(start_value.get_value());

  std::function<bool()> condition;

  if(std::get<double>(step_value.get_value()) >= 0) {
    condition = [&]() -> bool { return i < std::get<double>(end_value.get_value()); };
  } else {
    condition = [&]() -> bool { return i > std::get<double>(end_value.get_value()); };
  }

  while(condition()) {
    context.symbol_table->set(
      std::get<std::string>(node.var_name_tok.value.value()),
      std::make_shared<Number>(i)
    );

    i += std::get<double>(step_value.get_value());

    res.register_(visit(node.body, context));
    if(res.error) return res;

  }

  return res.success(std::nullopt);
}

RTResult Interpreter::visit_WhileNode(const WhileNode& node, Context& context) const {
  RTResult res;

  while(true) {
    Number condition = res.register_(visit(node.condition, context));
    if(res.error) return res;

    if(!condition.is_true()) break;

    res.register_(visit(node.body, context));
    if(res.error) return res;
  }

  return res.success(std::nullopt);
}