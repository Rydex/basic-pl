#include "nodes.h"
#include "context.h"
#include "state/interpreter.h"
#include <memory>
#include "parser.h"

// visitors

RTResult VarAccessNode::accept(const Interpreter& visitor, Context& context) {
  return visitor.visit_VarAccessNode(*this, context);
}

RTResult VarAssignNode::accept(const Interpreter& visitor, Context& context) {
  return visitor.visit_VarAssignNode(*this, context);
}

RTResult BinOpNode::accept(const Interpreter& visitor, Context& context) {
  return visitor.visit_BinOpNode(*this, context);
}

RTResult UnaryOpNode::accept(const Interpreter& visitor, Context& context) {
  return visitor.visit_UnaryOpNode(*this, context);
}

RTResult NumberNode::accept(const Interpreter& visitor, Context& context) {
  return visitor.visit_NumberNode(*this, context);
}

RTResult IfNode::accept(const Interpreter& visitor, Context& context) {
  return visitor.visit_IfNode(*this, context);
}

RTResult ForNode::accept(const Interpreter& visitor, Context& context) {
  return visitor.visit_ForNode(*this, context);
}

RTResult WhileNode::accept(const Interpreter& visitor, Context& context) {
  return visitor.visit_WhileNode(*this, context);
}

// end visitors

UnaryOpNode::UnaryOpNode(
  const Token& op_tok,
  const std::shared_ptr<ASTNode>& node
): op_tok(op_tok), node(node) {
  pos_end = ::get_pos_end(node);
}

BinOpNode::BinOpNode(
  const std::shared_ptr<ASTNode>& left_node,
  const Token& op_tok,
  const std::shared_ptr<ASTNode>& right_node
): 
  left_node(left_node),
  op_tok(op_tok), 
  right_node(right_node) {
  pos_start = ::get_pos_start(left_node);
  pos_end = ::get_pos_end(right_node);
}