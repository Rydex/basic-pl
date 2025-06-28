#include "../position.h"
#include <optional>
#include "../context.h"
#include "../exception.h"

class Value {
protected:
  std::optional<Position> pos_start, pos_end;
  std::optional<Context> context;

public:
  Value();

  Value& set_pos(
    const std::optional<Position>& pos_start = std::nullopt,
    const std::optional<Position>& pos_end = std::nullopt
  );

  Value& set_context(
    const std::optional<Context>& context = std::nullopt
  );
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