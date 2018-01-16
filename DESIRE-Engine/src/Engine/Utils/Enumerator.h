#pragma once

#include <stdint.h>			// for int64_t
#include <type_traits>		// std::underlying_type

// --------------------------------------------------------------------------------------------------------------------
//	Helper class for enumerating and using enum class values in range based for loops without the need of static_cast
//	In order to work the enum class is required to have the NUM value
//	Usage:
//		for(auto i : Enumerator<EMyEnum>())
// --------------------------------------------------------------------------------------------------------------------

template<typename ENUM>
class Enumerator
{
public:
	struct Iterator
	{
		union
		{
			ENUM										enumVal;
			typename std::underlying_type<ENUM>::type	intVal;
		};

		explicit Iterator(ENUM value)
			: enumVal(value)
		{

		}

		explicit Iterator(typename std::underlying_type<ENUM>::type value)
			: intVal(value)
		{

		}

		inline operator ENUM() const
		{
			return enumVal;
		}

		inline operator int64_t() const
		{
			return intVal;
		}

		inline operator size_t() const
		{
			return (size_t)intVal;
		}

		inline bool operator !=(Iterator rhs) const
		{
			return intVal != rhs.intVal;
		}

		inline bool operator !=(ENUM other) const
		{
			return enumVal != other;
		}

		inline void operator ++()
		{
			++intVal;
		}

		inline Iterator operator *() const
		{
			return *this;
		}
	};

	inline Iterator begin() const
	{
		return Iterator(0);
	}

	inline Iterator end() const
	{
		return Iterator(ENUM::NUM);
	}
};
