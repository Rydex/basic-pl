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
  ParseResult factor();
  ParseResult term();
  ParseResult expr();
  ParseResult bin_op(
    const std::vector<std::string>& ops,
    const std::function<ParseResult()>& func
  );
};


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