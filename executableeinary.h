#pragma once

#include <vector>
#include "advanced_enum.h"

enum_((Instruction)(Stop)(Load)(Store)(Jump)(AddI)(AddF)(Add2I)(Add2F)(SubI)(SubF)(Sub2I)(Sub2F)(PrintI)(PrintF)(MulI)(MulF)(Mul2I)(Mul2F))

class ExecutableBinary
{
public:

	template <class T>
	T peek(std::size_t pos) const
	{
		T result;
		memcpy(&result, _buffer.data() + pos, sizeof(T));
		return result;
	}

	template <class T>
	T getas(std::size_t & pos) const
	{
		T result;
		memcpy(&result, _buffer.data() + pos, sizeof(T));
		pos += sizeof(T);
		return result;
	}

	const unsigned char* get(std::size_t& pos) const
	{
		return _buffer.data() + pos;
	}

	template <class T>
	void setas(std::size_t & pos, T t)
	{
		if (_buffer.size() < pos + sizeof(T))
			_buffer.resize((std::size_t)(1.4 * double(pos + (std::size_t)sizeof(T))));
		memcpy(_buffer.data() + pos, &t, sizeof(T));
		pos += sizeof(T);
	}
private:
	std::vector<unsigned char> _buffer;
};

