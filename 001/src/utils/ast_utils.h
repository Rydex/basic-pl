#ifndef AST_UTILS_H
#define AST_UTILS_H

#include "../nodes.h"
#include "../parser.h"
#include <stdexcept>
#include <variant>

NodeVariant convert(const RegisterVariant& val);

#endif
