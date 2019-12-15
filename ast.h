#pragma once

#include <variant>
#include <string>
#include <optional>
#include <vector>
#include "standard.h"
#include "advanced_enum.h"
#include <typeinfo>

class AST;

struct ParseFailureInfo
{
	ParseFailureInfo(const char* m) :message(m) {}
	const char* message;
};

struct FloatTreeItem
{
	mtd::Float value;
	auto get_tuple() const { return std::make_tuple(value); }
};

struct IntTreeItem
{
	mtd::Int value;
	auto get_tuple() const { return std::make_tuple(value); }
};

struct IDTreeItem
{
	std::string ID;
	auto get_tuple() const { return std::make_tuple(ID); }
};

enum_((Operator)(Add)(Subtract)(Multiply)(Divide)(None))

struct FactorTreeItem
{
	mtd::PointerWrapper<AST> first;
	std::optional<mtd::PointerWrapper<AST>> second;
	Operator op;
	auto get_tuple() const { return std::make_tuple(first, op, second); }
};

struct FunctionCallTreeItem
{
	std::string function_name;
	std::vector<mtd::PointerWrapper<AST>> arguments;
	auto get_tuple() const { return std::make_tuple(function_name, arguments); }
};

enum_((PrimitiveType)(Float)(Integer)(None))
static_assert(is_advanced_enum_v<PrimitiveType>);

struct AssignmentTreeItem
{
	std::string varname;
	mtd::PointerWrapper<AST> expression;
	PrimitiveType declarationtype;
	auto get_tuple() const { return std::make_tuple(varname, expression, declarationtype); }
};

struct StatementsTreeItem
{
	std::vector<AST> statements;
	auto get_tuple() const { return std::make_tuple(statements); }
};

class AST
{
public:
	AST() = default;
	AST(const AST& other) :tree(other.tree) {};
	AST(AST&& t) = default;
	AST& operator=(AST ast)
	{
		using Variant = decltype(tree);
		tree.~Variant();
		new (&tree) Variant(std::move(ast.tree));
		return *this;
	}
	template <class T, class = std::enable_if_t<!std::is_same_v<std::decay_t<T>,AST>>>
	AST(T&& t) :tree(std::forward<T>(t)) {}
	friend std::ostream& operator<<(std::ostream& o, const AST& ast);
	std::variant<FloatTreeItem, IntTreeItem, IDTreeItem, FactorTreeItem, FunctionCallTreeItem, AssignmentTreeItem, StatementsTreeItem> tree;
};

struct Test
{
	Test(int) {}
	auto get_tuple() const { return std::make_tuple(1); }
};
inline std::ostream& operator<<(std::ostream& o, const AST& ast)
{
	auto printer = [&o](const auto& x) { o << typeid(decltype(x)).name() << '<' << x.get_tuple() << '>'; };
	std::visit(printer, ast.tree);
	return o;
}
