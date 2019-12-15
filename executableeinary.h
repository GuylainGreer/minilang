#pragma once

#include <vector>
#include <algorithm>
#include "advanced_enum.h"

enum_((Instruction)(Stop)(Load)(Store)(Jump)
                   (AddI)(AddF)(Add2I)(Add2F)
	               (SubI)(SubF)(Sub2I)(Sub2F)
	               (MulI)(MulF)(Mul2I)(Mul2F)
	               (DivI)(DivF)(Div2I)(Div2F)
	               (Float2Int)(Int2Float)(PrintI)(PrintF))

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
		ensure_size(pos + sizeof(T));
		memcpy(_buffer.data() + pos, &t, sizeof(T));
		pos += sizeof(T);
	}
	template <class T>
	void push_back(T t)
	{
		auto pos = _buffer.size();
		setas(pos, t);
	}
private:
	std::vector<unsigned char> _buffer;
	void ensure_capacity(std::size_t capacity)
	{
		if (_buffer.capacity() <= capacity)
			_buffer.reserve(std::max(capacity, std::size_t(1.4 * _buffer.capacity())));
	}
	void ensure_size(std::size_t size)
	{
		if (_buffer.size() <= size)
		{
			ensure_capacity(size);
			_buffer.resize(size);
		}
	}
};

