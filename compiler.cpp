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
	while (std::isspace(*code))
		code++;
	return code;
}

RAST parse_statement(const char* code);
RAST parse_assignment(const char* code);
RAST parse_expression(const char* code);
RAST parse_expression_with_parens(const char* code);
RAST parse_factor(const char* code);
RAST parse_factor_opt(const char* code);
RAST parse_term(const char* code);
RAST parse_term_opt(const char* code);
RAST parse_number(const char* code);
RAST parse_identifier(const char* code);
RAST parse_typename(const char* code);
RAST parse_functioncall(const char* code);
RAST parse_functiondef(const char* code);

//Only handles positive numbers, that may be integer or fractional
RAST parse_number(const char* code)
{
	auto c = *code;
	if (!std::isdigit(c))
		return { ParseFailureInfo("Not a number"), code };
	mtd::Int integer = 0;

	while (true)
	{
		if (c == '.')
			break;
		if (!std::isdigit(c))
			return { IntTreeItem{ integer }, code };
		integer = integer * 10 + (c - '0');
		c = *code++;
	}
	code++;
	std::size_t fraction = 0;
	std::size_t digits = 0;
	while (true)
	{
		if (!std::isdigit(c))
		{
			if (digits == 0)
				return { FloatTreeItem{ (double)integer }, code };
			else
				return { FloatTreeItem{ (double)integer + (double)fraction / double(1 << (digits + 1)) }, code };
		}
		digits++;
		fraction = fraction * 10 + (c - '0');
		c = *code++;
	}
}

std::pair<mtd::ReturnValue<std::string, ParseFailureInfo>, const char*> parse_identifier_raw(const char* code)
{
	auto c = *code;
	if (!std::isalnum(c))
		return { ParseFailureInfo("Expected an identifier"), code };
	if (isdigit(c))
		return { ParseFailureInfo("Identifier may not start with a digit"), code };
	std::string ID;
	ID.reserve(16);
	ID += c;
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

RAST parse_factor(const char* code_orig)
{
	auto code = code_orig;
	FactorTreeItem factor;
	if (RAST number = parse_number(code); number.value.success)
	{
		factor.first = std::move(number.value.get_value());
		code = number.code;
	}
	else if (RAST id = parse_identifier(code); number.value.success)
	{
		factor.first = std::move(id.value.get_value());
		code = id.code;
	}
	else if (RAST fc = parse_functioncall(code); fc.value.success)
	{
		factor.first = std::move(fc.value.get_value());
		code = fc.code;
	}
	else if (RAST ex = parse_expression_with_parens(code); ex.value.success)
	{
		factor.first = std::move(ex.value.get_value());
		code = ex.code;
	}
	else
	{
	}
	code = sw(code);
	if (*code == '*')
		factor.op = Operator::Multiply;
	else if (*code == '/')
		factor.op = Operator::Divide;
	else
		return { std::move(factor), code };
	auto result = parse_factor(code);
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
	FactorTreeItem term;
	if (result.value.success)
		term.first = std::move(result.value.get_value());
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
	AssignmentTreeItem a;
	//Right now only floats and ints
	static const char* float_ = "float";
	static const char* int_ = "int";
	if (strcmp(code, float_) == 0)
	{
		a.declarationtype = PrimitiveType::Float;
		code += 5;
	}
	else if (strcmp(code, int_) == 0)
	{
		a.declarationtype = PrimitiveType::Integer;
		code += 3;
	}
	else
	{
		a.declarationtype = PrimitiveType::None;
	}

	code = sw(code);
	auto identifier = parse_identifier_raw(code);
	if (identifier.first.success)
	{
		a.varname = identifier.first.get_value();
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
	a.expression = std::move(expr.value.get_value());
	return { std::move(a), expr.code };
}

AST Compiler::compile(const char* code) const
{
	return AST();
}
