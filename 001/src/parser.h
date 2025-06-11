#ifndef PARSER
#define PARSER

#include <memory>
#include <optional>
#include <variant>
#include <vector>
#include "token.h"
#include "exception.h"

// nodes

struct NumberNode {
  std::optional<Token> tok;

  std::string as_string() const;
};

struct BinOpNode;

using NodeVariant = std::variant<NumberNode, std::shared_ptr<BinOpNode>>;

struct BinOpNode {
  NodeVariant left_node;
  Token op_tok;
  NodeVariant right_node;

  std::string as_string() const;
};

// end nodes

// parser class

class Parser {
private:
  std::vector<Token> tokens;
  std::optional<Token> cur_tok;
  int tok_idx = -1;

public:
  Parser(const std::vector<Token>& tokens);

  Token advance();
  NodeVariant parse();
  NodeVariant factor();
  NodeVariant term();
  NodeVariant expr();
};

class ParseResult;

using RegisterVariant = std::variant<
  ParseResult,
  NumberNode,
  std::shared_ptr<BinOpNode>,
  Token
>;

class ParseResult {
public:
  std::optional<Exception> error = std::nullopt;
  std::optional<NodeVariant> node = std::nullopt;
  RegisterVariant register_(const RegisterVariant& res);
  ParseResult& success(const NodeVariant& node);
  ParseResult& failure(const std::optional<Exception>& error);
};

// end parser

#endif