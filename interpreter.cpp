#include "interpreter.h"
#include "standard.h"
#include <iostream>

void check_ram(std::vector<unsigned char>& ram, std::size_t pos, std::size_t width)
{
	std::size_t size = pos + width;
	if (ram.size() < size)
		ram.resize(size);
}

template <class T>
void set_ram(std::vector<unsigned char>& ram, std::size_t pos, T value)
{
	check_ram(ram, pos, sizeof(value));
	memcpy(ram.data() + pos, &value, sizeof(T));
}

template <class T>
T get_ram(std::vector<unsigned char>& ram, std::size_t pos)
{
	check_ram(ram, pos, sizeof(T));
	T result;
	memcpy(&result, ram.data() + pos, sizeof(T));
	return result;
}

void Interpreter::run(const ExecutableBinary & binary)
{
	std::size_t pos = 0;
	std::vector<unsigned char> accumulator(4);
	std::vector<unsigned char> ram(64);

	while (true)
	{
		std::cout << "Found instruction: " << ToString(binary.peek<Instruction>(pos)) << '\n';
		switch (binary.getas<Instruction>(pos))
		{
		case Instruction::Stop:
			goto done_binary;
		case Instruction::Load:
			set_ram(accumulator, 0, binary.getas<mtd::Word>(pos));
			break;
		case Instruction::Store:
		{
			set_ram(ram, binary.getas<mtd::Word>(pos), get_ram<mtd::Word>(accumulator, 0));
			break;
		}
		case Instruction::Jump:
			pos = binary.getas<mtd::Address>(pos);
			break;
		case Instruction::AddI:
		{
			auto address = binary.getas<mtd::Address>(pos);
			auto mem = get_ram<mtd::Int>(ram, address);
			auto acc = get_ram<mtd::Int>(accumulator, 0);
			set_ram(accumulator, 0, mem + acc);
			break;
		}
		case Instruction::AddF:
		{
			auto address = binary.getas<mtd::Address>(pos);
			auto mem = get_ram<mtd::Float>(ram, address);
			auto acc = get_ram<mtd::Float>(accumulator, 0);
			set_ram(accumulator, 0, mem + acc);
			break;
		}
		case Instruction::Add2I:
		{
			auto address = binary.getas<mtd::Address>(pos);
			auto mem1 = get_ram<mtd::Int>(ram, address);
			address = binary.getas<mtd::Address>(pos);
			auto mem2 = get_ram<mtd::Int>(ram, address);
			set_ram(accumulator, 0, mem1 + mem2);
			break;
		}
		case Instruction::Add2F:
		{
			auto address = binary.getas<mtd::Address>(pos);
			auto mem1 = get_ram<mtd::Float>(ram, address);
			address = binary.getas<mtd::Address>(pos);
			auto mem2 = get_ram<mtd::Float>(ram, address);
			set_ram(accumulator, 0, mem1 + mem2);
			break;
		}
		case Instruction::SubI:
		{
			auto address = binary.getas<mtd::Address>(pos);
			auto mem = get_ram<mtd::Int>(ram, address);
			auto acc = get_ram<mtd::Int>(accumulator, 0);
			set_ram(accumulator, 0, mem - acc);
			break;
		}
		case Instruction::SubF:
		{
			auto address = binary.getas<mtd::Address>(pos);
			auto mem = get_ram<mtd::Float>(ram, address);
			auto acc = get_ram<mtd::Float>(accumulator, 0);
			set_ram(accumulator, 0, mem - acc);
			break;
		}
		case Instruction::Sub2I:
		{
			auto address = binary.getas<mtd::Address>(pos);
			auto mem1 = get_ram<mtd::Int>(ram, address);
			address = binary.getas<mtd::Address>(pos);
			auto mem2 = get_ram<mtd::Int>(ram, address);
			set_ram(accumulator, 0, mem1 - mem2);
			break;
		}
		case Instruction::Sub2F:
		{
			auto address = binary.getas<mtd::Address>(pos);
			auto mem1 = get_ram<mtd::Float>(ram, address);
			address = binary.getas<mtd::Address>(pos);
			auto mem2 = get_ram<mtd::Float>(ram, address);
			set_ram(accumulator, 0, mem1 - mem2);
			break;
		}
		case Instruction::PrintI:
		{
			std::cout << get_ram<mtd::Int>(accumulator, 0) << std::endl;
			break;
		}
		case Instruction::PrintF:
		{
			std::cout << get_ram<mtd::Float>(accumulator, 0) << std::endl;
		}
		case Instruction::MulI:
		{
			auto address = binary.getas<mtd::Address>(pos);
			auto mem = get_ram<mtd::Int>(ram, address);
			auto acc = get_ram<mtd::Int>(accumulator, 0);
			set_ram(accumulator, 0, mem * acc);
			break;
		}
		case Instruction::MulF:
		{
			auto address = binary.getas<mtd::Address>(pos);
			auto mem = get_ram<mtd::Float>(ram, address);
			auto acc = get_ram<mtd::Float>(accumulator, 0);
			set_ram(accumulator, 0, mem * acc);
			break;
		}
		case Instruction::Mul2I:
		{
			auto address = binary.getas<mtd::Address>(pos);
			auto mem1 = get_ram<mtd::Int>(ram, address);
			address = binary.getas<mtd::Address>(pos);
			auto mem2 = get_ram<mtd::Int>(ram, address);
			set_ram(accumulator, 0, mem1 * mem2);
			break;
		}
		case Instruction::Mul2F:
		{
			auto address = binary.getas<mtd::Address>(pos);
			auto mem1 = get_ram<mtd::Float>(ram, address);
			address = binary.getas<mtd::Address>(pos);
			auto mem2 = get_ram<mtd::Float>(ram, address);
			set_ram(accumulator, 0, mem1 * mem2);
			break;
		}
		}
	}
done_binary:;
}
