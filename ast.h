#pragma once

#include <variant>
#include <string>
#include <optional>
#include <vector>
#include "standard.h"
#include "advanced_enum.h"

class AST;

struct ParseFailureInfo
{
	ParseFailureInfo(const char* m) :message(m) {}
	const char* message;
};

struct FloatTreeItem
{
	mtd::Float value;
};

struct IntTreeItem
{
	mtd::Int value;
};

struct IDTreeItem
{
	std::string ID;
};

enum_((Operator)(Add)(Subtract)(Multiply)(Divide))

struct FactorTreeItem
{
	mtd::PointerWrapper<AST> first;
	mtd::PointerWrapper<AST> second;
	Operator op;
};

struct FunctionCallTreeItem
{
	std::string function_name;
	std::vector<mtd::PointerWrapper<AST>> arguments;
};

enum_((PrimitiveType)(Float)(Integer)(None))

struct AssignmentTreeItem
{
	std::string varname;
	mtd::PointerWrapper<AST> expression;
	PrimitiveType declarationtype;
};

class AST
{
public:
	AST() = default;
	AST(const AST&) = default;
	template <class T, class = std::enable_if_t<!std::is_same_v<std::decay_t<T>,AST>>>
	AST(T&& t) :tree(std::forward<T>(t)) {}
private:
	std::variant<FloatTreeItem, IntTreeItem, IDTreeItem, FactorTreeItem, FunctionCallTreeItem, AssignmentTreeItem> tree;
};
