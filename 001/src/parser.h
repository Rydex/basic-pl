#ifndef PARSER
#define PARSER

#include <functional>
#include <memory>
#include <optional>
#include <variant>
#include <vector>
#include "token.h"
#include "exception.h"

// nodes

struct NumberNode {
  std::optional<Token> tok;
  Position pos_start = tok->pos_start.value();
  Position pos_end = tok->pos_end.value();

  NumberNode(const Token& token);

  std::string as_string() const;
};

struct BinOpNode;
struct UnaryOpNode;

// helpers
using SharedBin = std::shared_ptr<BinOpNode>;
using SharedUnary = std::shared_ptr<UnaryOpNode>;

using NodeVariant = std::variant<
  NumberNode,
  SharedUnary,
  SharedBin
>;

std::string stringify_node(const NodeVariant& node);

struct BinOpNode {
  NodeVariant left_node;
  Token op_tok;
  NodeVariant right_node;
  std::optional<Position> pos_start, pos_end;
  
  BinOpNode(
    const NodeVariant& left_node,
    const Token& op_tok,
    const NodeVariant& right_node
  );

  std::string as_string() const;
};

struct UnaryOpNode {
  Token op_tok;
  NodeVariant node;
  std::optional<Position> pos_start = op_tok.pos_start.value(), pos_end;

  UnaryOpNode(
    const Token& op_tok,
    const NodeVariant& node
  );

  std::string as_string() const;
};

// end nodes

class ParseResult;

// parser class

class Parser {
private:
  std::vector<Token> tokens;
  std::optional<Token> cur_tok;
  int tok_idx = -1;

public:
  Parser(const std::vector<Token>& tokens);

  Token advance();
  ParseResult parse();
  ParseResult atom();
  ParseResult factor();
  ParseResult term();
  ParseResult expr();
  ParseResult power();
  ParseResult bin_op(
    const std::function<ParseResult()>& func_a,
    const std::vector<std::string>& ops,
    const std::optional<std::function<ParseResult()>>& func_b = std::nullopt
  );
};

using RegisterVariant = std::variant<
  ParseResult,
  NumberNode,
  SharedBin,
  Token,
  SharedUnary
>;

class ParseResult {
public:
  std::optional<Exception> error = std::nullopt;
  std::optional<NodeVariant> node = std::nullopt;
  RegisterVariant register_(const RegisterVariant& res);
  ParseResult& success(const NodeVariant& node);
  ParseResult& failure(const std::optional<Exception>& error);
};

Position get_pos_end(const NodeVariant& node);
Position get_pos_start(const NodeVariant& node);

// end parser

#endif