#include <iostream>
#include "executableeinary.h"
#include "interpreter.h"

int main()
{
	ExecutableBinary binary;
	std::size_t pos = 0;
	binary.setas(pos, Instruction::Load2);
	binary.setas<unsigned short>(pos, 16);
	binary.setas(pos, Instruction::Store2);
	binary.setas(pos, 0u);
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
