#pragma once

#include <vector>
#include "advanced_enum.h"

enum_((Instruction)(Stop)(Load1)(Load2)(Load4)(Store1)(Store2)(Store4)(Jump)
				   (Add1)(Add2)(Add4I)(Add4F))

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
			_buffer.resize((std::size_t)(1.4 * (pos + sizeof(T))));
		memcpy(_buffer.data() + pos, &t, sizeof(T));
		pos += sizeof(T);
	}
private:
	std::vector<unsigned char> _buffer;
};

