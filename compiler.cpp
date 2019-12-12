#include "compiler.h"
#include "standard.h"
#include <cctype>
#include <utility>

using OAST = mtd::ReturnValue<AST, ParseFailureInfo>;
struct RAST
{
	template <class T>
	RAST(T && t, const char* code) :
		value(std::forward<T>(t)), code(std::move(code)) {}
	
	OAST value;
	const char* code;
};

const char* sw(const char* code)
{
	while (std::isspace((unsigned char)*code))
		code++;
	return code;
}

RAST parse_statement(const char* code);
RAST parse_assignment(const char* code);
RAST parse_expression(const char* code);
RAST parse_expression_with_parens(const char* code);
RAST parse_factor(const char* code);
RAST parse_term(const char* code);
RAST parse_number(const char* code);
RAST parse_identifier(const char* code);
RAST parse_functioncall(const char* code);
RAST parse_functiondef(const char* code);
RAST parse_statements(const char* code);

//Only handles positive numbers, that may be integer or fractional
RAST parse_number(const char* code_orig)
{
	auto code = code_orig;
	unsigned char first = *code;
	if (!std::isdigit(first))
		return { ParseFailureInfo("Not a number"), code_orig };

	mtd::Int integer = 0;
	while (true)
	{
		unsigned char c = *code;
		if (c == '.')
			break;
		if (!std::isdigit(c))
			return { IntTreeItem{ integer }, code };
		integer = integer * 10 + (c - '0');
		code++;
	}
	code++;
	std::size_t fraction = 0;
	std::size_t digits = 0;
	while (true)
	{
		unsigned char c = *code;
		if (!std::isdigit(c))
		{
			if (digits == 0)
				return { FloatTreeItem{ (double)integer }, code };
			else
				return { FloatTreeItem{ (double)integer + (double)fraction / double(1ull << (digits + 1)) }, code };
		}
		digits++;
		fraction = fraction * 10 + (c - '0');
		code++;
	}
}

std::pair<mtd::ReturnValue<std::string, ParseFailureInfo>, const char*> parse_identifier_raw(const char* code)
{
	unsigned char c = *code;
	if (!std::isalnum(c))
		return { ParseFailureInfo("Expected an identifier"), code };
	if (isdigit(c))
		return { ParseFailureInfo("Identifier may not start with a digit"), code };
	std::string ID;
	ID.reserve(16);
	c = *code++;
	while (std::isalnum(c))
	{
		ID += c;
		c = *code++;
	}
	return { ID, code };
}

RAST parse_identifier(const char* code)
{
	auto result = parse_identifier_raw(code);
	if (result.first.success)
	{
		return { IDTreeItem{ std::move(result.first.get_value()) }, result.second };
	}
	else
	{
		return { result.first.get_error(), result.second };
	}
}

RAST ReturnFirstSuccessfulParse(const char * code)
{
	return { ParseFailureInfo{"Failed to parse from options"}, code };
}

template <class ... ParseFunctions>
RAST ReturnFirstSuccessfulParse(const char * code, RAST(*func)(const char*), ParseFunctions ... parse_functions)
{
	auto result = func(code);
	if (result.value.success)
		return result;
	else
		return ReturnFirstSuccessfulParse(code, parse_functions...);
}

RAST parse_factor(const char* code_orig)
{
	auto code = code_orig;
	auto result = ReturnFirstSuccessfulParse(code, parse_number, parse_identifier, parse_functioncall, parse_expression_with_parens);
	if (!result.value.success)
		return result;
	code = sw(result.code);
	FactorTreeItem factor{ std::move(result.value.get_value()) };
	if (*code == '*')
		factor.op = Operator::Multiply;
	else if (*code == '/')
		factor.op = Operator::Divide;
	else
	{
		factor.op = Operator::None;
		return { std::move(factor), code };
	}
	result = parse_factor(code);
	if(result.value.success)
		factor.second = std::move(result.value.get_value());
	else
	{
		return { ParseFailureInfo("Expected a factor"), code_orig };
	}
	return { factor, code };
}

RAST parse_term(const char* code_orig)
{
	auto code = code_orig;
	auto result = parse_factor(code);

	if (!result.value.success)
		return result;
	FactorTreeItem term{ std::move(result.value.get_value()) };
	code = sw(result.code);
	if (*code == '+')
		term.op = Operator::Add;
	else if (*code == '-')
		term.op = Operator::Subtract;
	else
		return { term, code };
	code = sw(code + 1);
	result = parse_factor(code);
	if (result.value.success)
	{
		term.second = std::move(result.value.get_value());
		return { term, result.code };
	}
	else
	{
		return { ParseFailureInfo(""), code_orig };
	}
}

RAST parse_expression(const char* code_orig)
{
	return parse_term(code_orig);
}

RAST parse_expression_with_parens(const char* code_orig)
{
	auto code = code_orig;
	code = sw(code);
	if (*code != '(')
		return { ParseFailureInfo("expected '('"), code_orig };
	auto expression = parse_expression(code);
	if (!expression.value.success)
		return expression;
	code = sw(code);
	if (*code != ')')
		return { ParseFailureInfo("expected ')'"), code };
	expression.code = code + 1;
	return expression;
}

RAST parse_functioncall(const char* code_orig)
{
	auto code = code_orig;
	auto function_name = parse_identifier_raw(code);
	if (!function_name.first.success)
		return { ParseFailureInfo("not a valid function name"), code_orig };
	FunctionCallTreeItem fc;
	fc.function_name = std::move(function_name.first.get_value());
	code = function_name.second;
	code = sw(code);
	if (*code != '(')
		return { ParseFailureInfo("expected '(' for function call"), code_orig };
	code++;
	std::vector<mtd::PointerWrapper<AST>> arguments;
	while (true)
	{
		auto result = parse_expression(code);
		if (!result.value.success)
			break;
		arguments.emplace_back(std::move(result.value.get_value()));
		code = result.code;
	}
	code = sw(code);
	if (*code != ')')
		return { ParseFailureInfo("Expected ')' for function call"), code_orig };
	code++;
	fc.arguments = std::move(arguments);
	return { std::move(fc), code };
}

RAST parse_assignment(const char* code_orig)
{
	auto code = sw(code_orig);
	//Right now only floats and ints
	static const char* float_ = "float";
	static const char* int_ = "int";
	PrimitiveType declarationtype;
	if (strcmp(code, float_) == 0)
	{
		declarationtype = PrimitiveType::Float;
		code += 5;
	}
	else if (strcmp(code, int_) == 0)
	{
		declarationtype = PrimitiveType::Integer;
		code += 3;
	}
	else
	{
		declarationtype = PrimitiveType::None;
	}

	code = sw(code);
	auto identifier = parse_identifier_raw(code);
	std::string varname;
	if (identifier.first.success)
	{
		varname = std::move(identifier.first.get_value());
		code = identifier.second;
	}
	else
	{
		return { ParseFailureInfo("Expected identifier for assignment"), code_orig };
	}

	code = sw(code);
	if (*code != '=')
		return { ParseFailureInfo("Expected '=' for assignment"), code_orig };
	code = sw(code + 1);
	auto expr = parse_expression(code);
	if (!expr.value.success)
		return { expr.value.get_error(), code_orig };
	return { AssignmentTreeItem{std::move(varname), std::move(expr.value.get_value()), declarationtype}, expr.code };
}

RAST parse_statements(const char* code_orig)
{
	std::vector<AST> statements;
	auto code = code_orig;
	while (true)
	{
		auto result = parse_statement(code);
		if (result.value.success)
		{
			statements.push_back(result.value.get_value());
			//statements.emplace_back(std::move(result.value.get_value()));
			code = result.code;
			code = sw(code);
			if (*code == 0)
				return { StatementsTreeItem{statements}, code };
		}
		else
		{
			return { ParseFailureInfo("Statement error"), code_orig };
		}
	}
}

RAST parse_statement(const char* code_orig)
{
	auto code = code_orig;
	auto result = parse_assignment(code);
	if (result.value.success)
	{
		code = sw(result.code);
		if (*code != ';')
			return { ParseFailureInfo("Expected ';' after assignment"), code_orig };
		result.code = code + 1;
		return result;
	}
	result = parse_functioncall(code);
	if (result.value.success)
	{
		code = sw(result.code);
		if (*code != ';')
			return { ParseFailureInfo("Expected ';' after function call"), code_orig };
		result.code = code + 1;
		return result;
	}
	else
		return { ParseFailureInfo("Could not parse statement"), code };
}

AST Compiler::compile(const char* code) const
{
	auto result = parse_statements(code);
	if (result.value.success)
		return result.value.get_value();
	else
	{
		std::cout << "Parsing failed" << std::endl;
		return AST();
	}
}
