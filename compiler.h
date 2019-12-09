#pragma once

#include <string_view>
#include "ast.h"
#include "standard.h"

class Compiler
{
public:
	AST compile(const char* code) const;
private:
};
