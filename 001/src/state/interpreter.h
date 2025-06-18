#ifndef INTERPRETER
#define INTERPRETER

#include "../context.h"
#include "../nodes.h"
#include "../position.h"
#include "../exception.h"
#include <functional>

class Number;

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

  NumberPair added_to(const Number& other) const;
  NumberPair subbed_by(const Number& other) const;
  NumberPair multiplied_by(const Number& other) const;
  NumberPair divided_by(const Number& other) const;
  NumberPair powed_by(const Number& other) const;
  NumberPair modded_by(const Number& other) const;

  std::string as_string() const;
};

using RTVariant = std::variant<Number, int, double, std::string>;

class RTResult {
public:
  std::optional<RTVariant> value = std::nullopt;
  std::shared_ptr<Exception> error = nullptr;

  Number register_(const RTResult& res);
  RTResult& success(const RTVariant& value);
  RTResult& failure(const std::shared_ptr<Exception>& error);
};

class Interpreter {
public:
  RTResult visit(const std::shared_ptr<ASTNode>& node, Context& context);
  RTResult visit_NumberNode(const NumberNode& node, Context& context);
  RTResult visit_BinOpNode(const BinOpNode& node, Context& context);
  RTResult visit_UnaryOpNode(const UnaryOpNode& node, Context& context);
  RTResult visit_VarAccessNode(const VarAccessNode& node, Context& context);
  RTResult visit_VarAssignNode(const VarAssignNode& node, Context& context);
};


#endif