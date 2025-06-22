#ifndef INTERPRETER
#define INTERPRETER

#include "../context.h"
#include "../nodes.h"
#include "../position.h"
#include "../exception.h"
#include <functional>
#include <optional>
#include <string>

class Number;

const std::vector<std::string> builtins = {
  "null",
  "quit",
  "true",
  "false"
};

class Value;

using ValuePair = std::pair<
  std::shared_ptr<Value>,
  std::shared_ptr<Exception>
>;

class Value {
protected:
  std::optional<Position> pos_start, pos_end;
  std::optional<Context> context;

public:

  Value& set_pos(
    const std::optional<Position>& pos_start = std::nullopt,
    const std::optional<Position>& pos_end = std::nullopt
  );

  Value& set_context(
    const std::optional<Context>& context = std::nullopt
  );

  virtual ValuePair added_to(const std::shared_ptr<Value>& other) const;
  virtual ValuePair subbed_by(const std::shared_ptr<Value>& other) const;
  virtual ValuePair multiplied_by(const std::shared_ptr<Value>& other) const;
  virtual ValuePair divided_by(const std::shared_ptr<Value>& other) const;
  virtual ValuePair powed_by(const std::shared_ptr<Value>& other) const;
  virtual ValuePair modded_by(const std::shared_ptr<Value>& other) const;
  virtual ValuePair eq_comp(const std::shared_ptr<Value>& other) const;
  virtual ValuePair ne_comp(const std::shared_ptr<Value>& other) const;
  virtual ValuePair lt_comp(const std::shared_ptr<Value>& other) const;
  virtual ValuePair gt_comp(const std::shared_ptr<Value>& other) const;
  virtual ValuePair lte_comp(const std::shared_ptr<Value>& other) const;
  virtual ValuePair gte_comp(const std::shared_ptr<Value>& other) const;
  virtual ValuePair and_comp(const std::shared_ptr<Value>& other) const;
  virtual ValuePair or_comp(const std::shared_ptr<Value>& other) const;
  virtual ValuePair not_operator() const;

  virtual RTResult execute(const std::vector<std::shared_ptr<Value>>& args);

  virtual std::shared_ptr<Value> copy() { throw std::runtime_error("no copy method"); };
  inline bool is_true() { return false; }

  RTResult illegal_operation(
    const std::optional<std::shared_ptr<Value>>& other = std::nullopt
  ) const;

  virtual ~Value() {}
};

class Number : public Value {
protected:
  std::optional<Position> pos_start, pos_end;
  std::optional<Context> context;

public:
  double value;
  Number(double value);

  // setters
  Number& set_pos(
    const std::optional<Position>& pos_start = std::nullopt,
    const std::optional<Position>& pos_end = std::nullopt
  );
  Number& set_context(const std::optional<Context>& context = std::nullopt);
  inline TokenValue get_value() const { return value; };
  std::shared_ptr<Value> copy();

  // operations
  ValuePair added_to(const std::shared_ptr<Value>& other) const;
  ValuePair subbed_by(const std::shared_ptr<Value>& other) const;
  ValuePair multiplied_by(const std::shared_ptr<Value>& other) const;
  ValuePair divided_by(const std::shared_ptr<Value>& other) const;
  ValuePair powed_by(const std::shared_ptr<Value>& other) const;
  ValuePair modded_by(const std::shared_ptr<Value>& other) const;
  ValuePair eq_comp(const std::shared_ptr<Value>& other) const;
  ValuePair ne_comp(const std::shared_ptr<Value>& other) const;
  ValuePair lt_comp(const std::shared_ptr<Value>& other) const;
  ValuePair gt_comp(const std::shared_ptr<Value>& other) const;
  ValuePair lte_comp(const std::shared_ptr<Value>& other) const;
  ValuePair gte_comp(const std::shared_ptr<Value>& other) const;
  ValuePair and_comp(const std::shared_ptr<Value>& other) const;
  ValuePair or_comp(const std::shared_ptr<Value>& other) const;
  ValuePair not_operator() const;

  bool is_true() const;

  std::string as_string() const;
};

class Function : public Value {
public:
  std::string name;
  std::shared_ptr<ASTNode> body;
  std::vector<std::string> arg_names;
  Function(
    const std::shared_ptr<ASTNode>& body,
    const std::vector<std::string>& arg_names,
    const std::string& name = "<unnamed function>"
  )
    : name(name), body(body), arg_names(arg_names) {}

  RTResult execute(const std::vector<std::shared_ptr<Value>>& args) override;
};

using RTVariant = std::variant<
  std::shared_ptr<Value>, int, double, std::string
>;

class RTResult {
public:
  std::optional<RTVariant> value = std::nullopt;
  std::shared_ptr<Exception> error = nullptr;

  std::shared_ptr<Value> register_(const RTResult& res);
  RTResult& success(const std::optional<RTVariant>& value);
  RTResult& failure(const std::shared_ptr<Exception>& error);
};

class Interpreter {
public:
  // visitors
  RTResult visit(const std::shared_ptr<ASTNode>& node, Context& context) const;
  RTResult visit_NumberNode(const NumberNode& node, Context& context) const;
  RTResult visit_BinOpNode(const BinOpNode& node, Context& context) const;
  RTResult visit_UnaryOpNode(const UnaryOpNode& node, Context& context) const;
  RTResult visit_VarAccessNode(const VarAccessNode& node, Context& context) const;
  RTResult visit_VarAssignNode(const VarAssignNode& node, Context& context) const;
  RTResult visit_IfNode(const IfNode& node, Context& context) const;
  RTResult visit_ForNode(const ForNode& node, Context& context) const;
  RTResult visit_WhileNode(const WhileNode& node, Context& context) const;
  RTResult visit_FuncDefNode(const FuncDefNode& node, Context& context) const;
  RTResult visit_CallNode(const CallNode& node, Context& context) const;
};


#endif