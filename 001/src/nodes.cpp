#include "nodes.h"
#include "context.h"
#include "state/interpreter.h"
#include <memory>

// visitors

RTResult VarAccessNode::accept(const Interpreter& visitor, const Context& context) {
  return visitor.visit_VarAccessNode(*this, context);
}

RTResult VarAssignNode::accept(const Interpreter& visitor, const Context& context) {
  return visitor.visit_VarAssignNode(*this, context);
}

RTResult BinOpNode::accept(const Interpreter& visitor, const Context& context) {
  return visitor.visit_BinOpNode(*this, context);
}

RTResult UnaryOpNode::accept(const Interpreter& visitor, const Context& context) {
  return visitor.visit_UnaryOpNode(*this, context);
}

RTResult NumberNode::accept(const Interpreter& visitor, const Context& context) {
  return visitor.visit_NumberNode(*this, context);
}

// end visitors

UnaryOpNode::UnaryOpNode(
  const Token& op_tok,
  const ASTNode& node
): op_tok(op_tok), node(std::make_shared<ASTNode>(node)) {
  pos_end = get_pos_end(node);
}

NumberNode::NumberNode(const Token& token)
  : tok(token), pos_start(token.pos_start.value()), pos_end(token.pos_end.value()) {}

BinOpNode::BinOpNode(
  const ASTNode& left_node,
  const Token& op_tok,
  const ASTNode& right_node
): 
  left_node(std::make_shared<ASTNode>(left_node)),
  op_tok(op_tok), 
  right_node(std::make_shared<ASTNode>(right_node)) {
  pos_start = get_pos_start(left_node);
  pos_end = get_pos_end(right_node);
}