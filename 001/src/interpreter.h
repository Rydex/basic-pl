#ifndef INTERPRETER
#define INTERPRETER

#include "parser.h"
#include <functional>

class Interpreter {
public:
	void visit(const NodeVariant& node);
	void visit_NumberNode(const NumberNode& node);
	void visit_BinOpNode(const BinOpNode& node);
	void visit_UnaryOpNode(const UnaryOpNode& node);
};

#endif