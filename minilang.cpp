#include <iostream>
#include "executableeinary.h"
#include "interpreter.h"
#include "standard.h"

int main()
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

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
