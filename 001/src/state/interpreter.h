#ifndef INTERPRETER
#define INTERPRETER

#include "../context.h"
#include "../nodes.h"
#include "../position.h"
#include "../exception.h"
#include <functional>

class Number;

const std::vector<std::string> builtins = {
  "null",
  "quit",
  "true",
  "false"
};

using NumberPair = std::pair<
  std::optional<Number>,
  std::shared_ptr<Exception>
>;

class Number {
protected:
  double value;
  std::optional<Position> pos_start, pos_end;
  std::optional<Context> context;

public:
  Number(double value);

  // setters
  Number& set_pos(
    const std::optional<Position>& pos_start = std::nullopt,
    const std::optional<Position>& pos_end = std::nullopt
  );
  Number& set_context(const std::optional<Context>& context = std::nullopt);
  inline TokenValue get_value() const { return value; };
  Number copy();

  // operations
  NumberPair added_to(const Number& other) const;
  NumberPair subbed_by(const Number& other) const;
  NumberPair multiplied_by(const Number& other) const;
  NumberPair divided_by(const Number& other) const;
  NumberPair powed_by(const Number& other) const;
  NumberPair modded_by(const Number& other) const;
  NumberPair eq_comp(const Number& other) const;
  NumberPair ne_comp(const Number& other) const;
  NumberPair lt_comp(const Number& other) const;
  NumberPair gt_comp(const Number& other) const;
  NumberPair lte_comp(const Number& other) const;
  NumberPair gte_comp(const Number& other) const;
  NumberPair and_comp(const Number& other) const;
  NumberPair or_comp(const Number& other) const;
  NumberPair not_operator() const;

  bool is_true() const;

  std::string as_string() const;
};

using RTVariant = std::variant<Number, int, double, std::string>;

class RTResult {
public:
  std::optional<RTVariant> value = std::nullopt;
  std::shared_ptr<Exception> error = nullptr;

  Number register_(const RTResult& res);
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
};


#endif