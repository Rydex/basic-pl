#include "interpreter.h"
#include "parser.h"
#include <functional>
#include <iostream>
#include <stdexcept>

void Interpreter::visit(const NodeVariant& node) {
	std::function<void(const NodeVariant& node)> method = [this](const NodeVariant& node) {
		if(std::holds_alternative<NumberNode>(node)) {
			visit_NumberNode(std::get<NumberNode>(node));
		} else if(std::holds_alternative<SharedBin>(node)) {
			visit_BinOpNode(*std::get<SharedBin>(node));
		} else if(std::holds_alternative<SharedUnary>(node)) {
			visit_UnaryOpNode(*std::get<SharedUnary>(node));
		} else {
			throw std::runtime_error("no visit method defined");
		}
	};

	method(node);
}

void Interpreter::visit_NumberNode(const NumberNode& node) {
	std::cout << "found numbernode\n";
}

void Interpreter::visit_BinOpNode(const BinOpNode& node) {
	std::cout << "found binopnode\n";
	visit(node.left_node);
	visit(node.right_node);
}

void Interpreter::visit_UnaryOpNode(const UnaryOpNode& node) {
	std::cout << "found unaryopnode\n";
	visit(node.node);
}