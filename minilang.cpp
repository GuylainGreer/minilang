#include <iostream>
#include "executableeinary.h"
#include "interpreter.h"
#include "standard.h"
#include "compiler.h"

void test_interpreter()
{
	ExecutableBinary binary;
	std::size_t pos = 0;
	binary.setas(pos, Instruction::Load);
	binary.setas<mtd::Int>(pos, 32);
	binary.setas(pos, Instruction::Store);
	binary.setas(pos, mtd::Address(0));
	binary.setas(pos, Instruction::Load);
	binary.setas<std::int64_t>(pos, 31);
	binary.setas(pos, Instruction::AddI);
	binary.setas<std::size_t>(pos, 0);
	binary.setas(pos, Instruction::PrintI);
	binary.setas(pos, Instruction::Store);
	binary.setas<mtd::Address>(pos, 8);
	binary.setas(pos, Instruction::Mul2I);
	binary.setas<mtd::Address>(pos, 0);
	binary.setas<mtd::Address>(pos, 8);
	binary.setas(pos, Instruction::PrintI);
	binary.setas<Instruction>(pos, Instruction::Stop);

	Interpreter interpreter;
	interpreter.run(binary);
}

int main()
{
	auto result = Compiler().compile("a = 1;");
	std::cout << result << std::endl;
}