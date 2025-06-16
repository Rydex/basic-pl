#ifndef PARSER
#define PARSER

#include <functional>
#include <memory>
#include <optional>
#include <variant>
#include <vector>
#include "token.h"
#include "exception.h"
#include "nodes.h"

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
  SharedUnary,
  SharedAssign,
  VarAccessNode
>;

class ParseResult {
public:
  std::shared_ptr<Exception> error = nullptr;
  std::optional<NodeVariant> node = std::nullopt;
  RegisterVariant register_(const RegisterVariant& res);
  ParseResult& success(const NodeVariant& node);
  ParseResult& failure(const std::shared_ptr<Exception>& error);
};

Position get_pos_end(const NodeVariant& node);
Position get_pos_start(const NodeVariant& node);

// end parser

#endif