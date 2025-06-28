#ifndef INTERPRETER
#define INTERPRETER

#include "../context.h"
#include "../nodes.h"
#include "../position.h"
#include "../exception.h"
#include "values.h"
#include <functional>

const std::vector<std::string> builtins = {
  "null",
  "quit",
  "true",
  "false"
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
  RTResult visit_ForNode(const ForNode& node, Context& context) const;
  RTResult visit_WhileNode(const WhileNode& node, Context& context) const;
  RTResult visit_FuncDefNode(const FuncDefNode& node, Context& context) const;
  RTResult visit_CallNode(const CallNode& node, Context& context) const;
};


#endif