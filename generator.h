#pragma once

#include "executableeinary.h"
#include "ast.h"

class Generator
{
	ExecutableBinary generate(const AST& ast);
};