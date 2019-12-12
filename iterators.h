#pragma once

#include <cstdint>

template <class T>
class RangeIterator
{
	T i;

	T operator*() const { return i; }
	RangeIterator & operator++() { ++i; return *this; }
	RangeIterator operator++(int) { auto o = *this; ++* this; return o; }
	RangeIterator & operator--() { --i; return *this; }
	RangeIterator operator--(int) { auto o = *this; --* this; return o; }
	RangeIterator operator+(std::int64_t d) const { return T{ std::int64_t(i) + d }; }
	RangeIterator operator-(std::int64_t d) const { return T{ std::int64_t(i) - d }; }
	RangeIterator & operator+=(std::int64_t d) { i = T{ std::int64_t(i) + d }; return *this; }
	RangeIterator & operator-=(std::int64_t d) { i = T{ std::int64_t(i) - d }; return *this; }
};

template <class T>
class Range
{
	T begin, end;
	Range(T end) :begin(0), end(end) {}
	Range(T begin, T end) :begin(begin), end(end) {}
};

