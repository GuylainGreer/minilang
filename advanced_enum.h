#pragma once

#include <string>
#include "iterators.h"

#define _def_enum(...) _def_enum_impl_ADD_END(_def_enum_impl_LIST __VA_ARGS__) }

#define _def_enum_impl_ADD_END(...) _def_enum_impl_ADD_END2(__VA_ARGS__)
#define _def_enum_impl_ADD_END2(...) __VA_ARGS__ ## _END

#define _def_enum_impl_LIST(...) enum class __VA_ARGS__ { _def_enum_impl_LIST_1

#define _def_enum_impl_LIST_1(...) __VA_ARGS__ , _def_enum_impl_LIST_2
#define _def_enum_impl_LIST_2(...) __VA_ARGS__ , _def_enum_impl_LIST_1

#define _def_enum_impl_LIST_END
#define _def_enum_impl_LIST_1_END
#define _def_enum_impl_LIST_2_END

template <class>
struct EnumLength : std::integral_constant<unsigned, -1> {};

#define _def_enum_length(...) _def_enum_length_impl_ADD_END(_def_enum_length_impl_LIST __VA_ARGS__) 0>{}

#define _def_enum_length_impl_ADD_END(...) _def_enum_length_impl_ADD_END2(__VA_ARGS__)
#define _def_enum_length_impl_ADD_END2(...) __VA_ARGS__ ## _END

#define _def_enum_length_impl_LIST(...) template <> struct EnumLength<__VA_ARGS__> : std::integral_constant<unsigned, _def_enum_length_impl_LIST_1

#define _def_enum_length_impl_add_one(...) 1
#define _def_enum_length_impl_LIST_1(...) _def_enum_length_impl_add_one( __VA_ARGS__ ) + _def_enum_length_impl_LIST_2
#define _def_enum_length_impl_LIST_2(...) _def_enum_length_impl_add_one( __VA_ARGS__ ) + _def_enum_length_impl_LIST_1

#define _def_enum_length_impl_LIST_END
#define _def_enum_length_impl_LIST_1_END
#define _def_enum_length_impl_LIST_2_END

#define _def_enum_names(...) _def_enum_names_impl_ADD_END(_def_enum_names_impl_LIST __VA_ARGS__) }; return names[(unsigned)e]; }

#define _def_enum_names_impl_ADD_END(...) _def_enum_names_impl_ADD_END2(__VA_ARGS__)
#define _def_enum_names_impl_ADD_END2(...) __VA_ARGS__ ## _END

#define _def_enum_names_impl_LIST(...) inline const std::string & ToString( __VA_ARGS__ e) { static std::string names[] = { _def_enum_names_impl_LIST_1

#define _def_enum_names_impl_LIST_1(...) #__VA_ARGS__, _def_enum_names_impl_LIST_2
#define _def_enum_names_impl_LIST_2(...) #__VA_ARGS__, _def_enum_names_impl_LIST_1

#define _def_enum_names_impl_LIST_END
#define _def_enum_names_impl_LIST_1_END
#define _def_enum_names_impl_LIST_2_END

#define enum_(...) _def_enum(__VA_ARGS__) ; _def_enum_length(__VA_ARGS__) ; _def_enum_names(__VA_ARGS__)

template <class Enum>
std::size_t ENUM_LENGTH = EnumLength<Enum>::value;

template <class T>
struct EnumIterator : RangeIterator<std::size_t>
{
	template <class ... Args>
	EnumIterator(std::size_t i): RangeIterator<std::size_t>(i) {}
	T operator*() const { return (T)RangeIterator<std::size_t>::operator*(); }
};

template <class T>
struct EnumRange
{
	EnumIterator<T> begin() { return EnumIterator<T>(0); }
	EnumIterator<T> end() { return EnumIterator<T>(ENUM_LENGTH<T>); }
};

