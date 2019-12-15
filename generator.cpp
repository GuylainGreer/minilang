#include "generator.h"
#include "advanced_enum.h"
#include <array>
#include <map>

struct GenerationContext
{
	ExecutableBinary code;
	mtd::Address stack_size;
	std::vector<PrimitiveType> stack_types;
	std::map<std::string, std::pair<mtd::Address, PrimitiveType>> variables;
};

void generate(const AST& ast, GenerationContext&);

void generate(const IDTreeItem& ast, GenerationContext& context)
{
	context.code.push_back(Instruction::Load);
	const auto& var = context.variables[ast.ID];
	context.code.push_back(var.first);
	context.code.push_back(Instruction::Store);
	context.code.push_back(context.stack_size);
	context.stack_types.push_back(var.second);
	context.stack_size++;
}

void generate(const FloatTreeItem& tree, GenerationContext& context)
{
	context.code.push_back(Instruction::Load);
	context.code.push_back(tree.value);
	context.code.push_back(Instruction::Store);
	context.code.push_back(sizeof(mtd::Float) * context.stack_size);
	context.stack_types.push_back(PrimitiveType::Float);
	context.stack_size++;
}

void generate(const IntTreeItem& tree, GenerationContext& context)
{
	context.code.push_back(Instruction::Load);
	context.code.push_back(tree.value);
	context.code.push_back(sizeof(mtd::Int) * context.stack_size);
	context.stack_types.push_back(PrimitiveType::Integer);
	context.stack_size++;
}

void generate(const AssignmentTreeItem& tree, GenerationContext& context)
{
	generate(tree.expression, context);
	auto iter = context.variables.find(tree.varname);
	if (iter == context.variables.end())
	{
		assert(tree.declarationtype != PrimitiveType::None);
		//Create new variable
		context.variables[tree.varname] =
			std::make_pair(context.stack_size - 1, tree.declarationtype);
	}
	else
	{
		assert(tree.declarationtype == PrimitiveType::None);
		mtd::Address assign_address = iter->second.first;
		context.stack_size--;
		context.code.push_back(Instruction::Store);
		context.code.push_back(assign_address);
	}
}

void generate(const FunctionCallTreeItem& tree, GenerationContext& context)
{
	assert(false);
}

void generate(const StatementsTreeItem& tree, GenerationContext& context)
{
	for (const auto& statement : tree.statements)
		generate(statement, context);
}

void generate(const AST& ast, GenerationContext& context)
{
	auto visitor = [&context](const auto& tree)
	{
		generate(tree, context);
	};
	std::visit(visitor, ast.tree);
}

void generate(const FactorTreeItem& tree, GenerationContext& context)
{
	generate(tree.first, context);
	if (tree.op == Operator::None)
		return;
	generate(*tree.second, context);
	//(Add)(Subtract)(Multiply)(Divide)(None)
	//Check the types of the top 2 stack values and possibly convert into -> float
	//for the binary operation
	if (*(context.stack_types.end() - 2) != *(context.stack_types.end() - 3))
	{
		auto first_type = *(context.stack_types.end() - 2);
		mtd::Address convert_address, float_address;
		if (first_type == PrimitiveType::Integer)
		{
			convert_address = context.stack_size - 2;
			float_address = context.stack_size - 1;
		}
		else
		{
			convert_address = context.stack_size - 1;
			float_address = context.stack_size - 2;
		}
		context.code.push_back(Instruction::Load);
		context.code.push_back(convert_address);
		context.code.push_back(Instruction::Int2Float);
		static const std::array<Instruction, ENUM_LENGTH<Operator> * 2> lookup =
		{Instruction::Add2F, Instruction::Sub2F,
			Instruction::Mul2F, Instruction::Div2F };
		context.code.push_back(lookup[unsigned(tree.op)]);
		context.code.push_back(float_address);
		context.code.push_back(Instruction::Store);
		context.code.push_back(context.stack_size - 2);
		context.stack_size--;
	}
	else
	{
		static const std::array<Instruction, ENUM_LENGTH<Operator> * 2> lookup =
		{ Instruction::Add2I, Instruction::Add2F,
		Instruction::Sub2I, Instruction::Sub2F,
		Instruction::Mul2I, Instruction::Mul2F,
		Instruction::Div2I, Instruction::Div2F };
		context.code.push_back(lookup[(unsigned)tree.op * 2 +
			(context.stack_types.back() == PrimitiveType::Float)]);
		context.code.push_back(Instruction::Store);
		context.code.push_back(context.stack_size - 2);
		context.stack_size--;
	}
}

ExecutableBinary Generator::generate(const AST& ast)
{
	return ExecutableBinary();
}
