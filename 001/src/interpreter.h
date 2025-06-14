#ifndef INTERPRETER
#define INTERPRETER

#include "context.h"
#include "parser.h"
#include "position.h"
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

  Number& set_pos(
    const std::optional<Position>& pos_start = std::nullopt,
    const std::optional<Position>& pos_end = std::nullopt
  );
  Number& set_context(const std::optional<Context>& context = std::nullopt);

  NumberPair added_to(const Number& other) const;
  NumberPair subbed_by(const Number& other) const;
  NumberPair multiplied_by(const Number& other) const;
  NumberPair divided_by(const Number& other) const;
  NumberPair powed_by(const Number& other) const;
  NumberPair modded_by(const Number& other) const;

  std::string as_string() const;
};

class RTResult {
public:
  std::optional<Number> value = std::nullopt;
  std::shared_ptr<Exception> error = nullptr;

  Number register_(const RTResult& res);
  RTResult& success(const Number& value);
  RTResult& failure(const std::shared_ptr<Exception>& error);
};

class Interpreter {
public:
  RTResult visit(const NodeVariant& node, const Context& context);
  RTResult visit_NumberNode(const NumberNode& node, const Context& context);
  RTResult visit_BinOpNode(const BinOpNode& node, const Context& context);
  RTResult visit_UnaryOpNode(const UnaryOpNode& node, const Context& context);
};


#endif