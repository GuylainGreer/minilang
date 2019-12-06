#include "interpreter.h"
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
	std::vector<unsigned char> ram1(16), ram2(32), ram4(64);

	while (true)
	{
		std::cout << "Found instruction: " << ToString(binary.peek<Instruction>(pos)) << '\n';
		switch (binary.getas<Instruction>(pos))
		{
		case Instruction::Stop:
			goto done_binary;
		case Instruction::Load1:
			memcpy(accumulator.data(), binary.get(pos), 1);
			pos += 1;
			break;
		case Instruction::Load2:
			memcpy(accumulator.data(), binary.get(pos), 2);
			pos += 2;
			break;
		case Instruction::Load4:
			memcpy(accumulator.data(), binary.get(pos), 4);
			pos += 4;
			break;
		case Instruction::Store1:
		{
			auto address = binary.getas<unsigned>(pos);
			check_ram(ram1, address, 1);
			memcpy(ram1.data() + address, accumulator.data(), 1);
			break;
		}
		case Instruction::Store2:
		{
			auto address = binary.getas<unsigned>(pos) * 2;
			check_ram(ram2, address, 2);
			memcpy(ram2.data() + address, accumulator.data(), 2);
			break;
		}
		case Instruction::Store4:
		{
			auto address = binary.getas<unsigned>(pos) * 4;
			check_ram(ram4, address, 4);
			memcpy(ram4.data() + address, accumulator.data(), 4);
			break;
		}
		case Instruction::Jump:
			pos = binary.getas<unsigned>(pos);
			break;
		case Instruction::Add1:
		{
			auto address = binary.getas<unsigned>(pos);
			auto mem = get_ram<std::int8_t>(ram1, pos);
			auto acc = get_ram<std::int8_t>(accumulator, 0);
			set_ram(accumulator, 0, mem + acc);
			break;
		}
		case Instruction::Add2:
		{
			auto address = binary.getas<unsigned>(pos);
			auto mem = get_ram<std::int16_t>(ram2, pos);
			auto acc = get_ram<std::int16_t>(accumulator, 0);
			set_ram(accumulator, 0, mem + acc);
		}
		case Instruction::Add4I:
		{
			auto address = binary.getas<unsigned>(pos);
			auto mem = get_ram<std::int32_t>(ram2, pos);
			auto acc = get_ram<std::int32_t>(accumulator, 0);
			set_ram(accumulator, 0, mem + acc);
		}
		case Instruction::Add4F:
		{
			auto address = binary.getas<unsigned>(pos);
			auto mem = get_ram<float>(ram2, pos);
			auto acc = get_ram<float>(accumulator, 0);
			set_ram(accumulator, 0, mem + acc);
		}
		}
	}
done_binary:;
}
