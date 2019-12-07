#pragma once

#include <cstdint>

namespace mtd
{
	using Int = std::int64_t;
	using Float = double;
	using Address = std::uint64_t;
	using Byte = std::uint8_t;
	using Word = std::uint64_t;

	template <class T, class U>
	T sc(U && t) { return static_cast<T>(std::forward<U>(t)); }
	template <class T, class U>
	T rc(U&& t) { return reinterpret_cast<T>(std::forward<U>(t)); }
	template <class T, class U>
	T dc(U&& t) { return dynamic_cast<T>(std::forward<U>(t)); }
}
