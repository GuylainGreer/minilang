#pragma once

#include <algorithm>
#include <cstdint>
#include <memory>
#include <cassert>
#include <vector>
#include <iostream>
#include <optional>

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

	struct Exception
	{
		virtual ~Exception() {}
		virtual const char* what() = 0;
	};

	template <class T, class ErrorT = std::shared_ptr<Exception>>
	struct ReturnValue
	{
		inline void swap(ReturnValue& left, ReturnValue& right)
		{
			if (left.success == right.success)
			{
				if (left.success)
					std::swap(left.get_value(), right.get_value());
				else
					std::swap(left.get_error(), right.get_error());
			}
			else
			{
				auto tmp = std::move(left);
				left = std::move(right);
				right = std::move(tmp);
			}
		}
		ReturnValue(T t)
		{
			new (value) T(std::move(t));
			success = true;
		}
		ReturnValue(ErrorT error)
		{
			new (value) ErrorT(std::move(error));
			success = false;
		}
		~ReturnValue()
		{
			if (success)
				rc<T*>(value)->~T();
			else
				rc<ErrorT*>(value)->~ErrorT();
		}
		ReturnValue(const ReturnValue& other)
		{
			success = other.success;
			if (success)
				get_value() = other.get_value();
			else
				get_error() = other.get_error();
		}
		ReturnValue(ReturnValue&& other)
		{
			success = std::move(other.success);
			if (success)
				get_value() = std::move(other.get_value());
			else
				get_error() = std::move(other.get_error());
		}
		ReturnValue& operator=(ReturnValue other)
		{
			swap(*this, other);
			return *this;
		}
		T& get_value()
		{
			assert(success);
			return *rc<T*>(value);
		}

		const T& get_value() const
		{
			assert(success);
			return *rc<const T*>(value);
		}

		ErrorT& get_error()
		{
			assert(!success);
			return *rc<ErrorT*>(value);
		}
		const ErrorT & get_error() const
		{
			assert(!success);
			return *rc<const ErrorT*>(value);
		}
		mtd::Byte value[std::max(sizeof(T), sizeof(ErrorT))];
		bool success;
	};

	template <class T>
	class PointerWrapper
	{
		template <class>
		struct IsPointerWrapper : std::false_type {};
		template <class U>
		struct IsPointerWrapper<PointerWrapper<U>> : std::true_type {};
	public:
		inline friend void swap(PointerWrapper& left, PointerWrapper& right)
		{
			std::swap(left.value, right.value);
		}

		PointerWrapper() = delete;
		PointerWrapper(PointerWrapper&& other) :value(other.value.release()) {}
		PointerWrapper(const PointerWrapper& other)
		{
			if (other.value)
				value = std::make_unique<T>(*other.value);
		}
		template <class U, class = std::enable_if_t<!IsPointerWrapper<std::decay_t<U>>::value>>
		PointerWrapper(U&& u) : value(std::make_unique<T>(std::forward<U>(u))) {}

		PointerWrapper& operator=(PointerWrapper other)
		{
			swap(*this, other);
			return *this;
		}
		T& get() { return *value; }
		const T& get() const { return *value; }
		operator T() const { return *value; }

	private:
		std::unique_ptr<T> value;
	};

	template <class T>
	std::ostream& operator<<(std::ostream& o, const PointerWrapper<T>& pw)
	{
		return o << pw.get();
	}
}

template <class ... Args>
std::ostream& operator<<(std::ostream& o, const std::tuple<Args...>& args)
{
	o << "tuple{";
	auto helper = [&o](const auto& ... x) { (o << ... << x) << ", "; };
	std::apply(helper, args);
	return o << "}";
}

template <class ... Args>
std::ostream& operator<<(std::ostream& o, const std::vector<Args...>& v)
{
	o << "v{";
	for (const auto& vv : v)
		o << vv << ", ";
	return o << "}";
}

template <class ... Args>
std::ostream& operator<<(std::ostream& o, const std::optional<Args...>& opt)
{
	if (opt)
		return o << *opt;
	else
		return o << "[empty optional]";
}