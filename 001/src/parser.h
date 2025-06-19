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
    const std::vector<std::pair<std::string, std::string>>& ops,
    const std::optional<std::function<ParseResult()>>& func_b = std::nullopt
  );
};

using RegisterVariant = std::variant<
  std::shared_ptr<ASTNode>,
  ParseResult
>;

class ParseResult {
public:
  int advance_count = 0;

  std::shared_ptr<Exception> error = nullptr;
  std::shared_ptr<ASTNode> node = nullptr;
  std::shared_ptr<ASTNode> register_(const ParseResult& res);
  void register_advance();
  ParseResult& success(const std::shared_ptr<ASTNode>& node);
  ParseResult& failure(const std::shared_ptr<Exception>& error);

};

Position get_pos_end(const std::shared_ptr<ASTNode>& node);
Position get_pos_start(const std::shared_ptr<ASTNode>& node);

// end parser

#endif