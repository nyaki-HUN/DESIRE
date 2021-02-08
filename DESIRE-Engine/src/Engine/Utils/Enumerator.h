#pragma once

#include <type_traits>		// std::underlying_type

// --------------------------------------------------------------------------------------------------------------------
//	Helper class for enumerating and using enum class values in range based for loops without the need of static_cast
//	In order to work the enum class is required to have the 'Num' value
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

		operator ENUM() const					{ return enumVal; }
		operator int64_t() const				{ return intVal;}
		operator size_t() const					{ return static_cast<size_t>(intVal); }

		bool operator !=(Iterator rhs) const	{ return intVal != rhs.intVal; }
		bool operator !=(ENUM other) const		{ return enumVal != other; }

		void operator ++()						{ ++intVal; }
		Iterator operator *() const				{ return *this; }
	};

	Iterator begin() const	{ return Iterator(0); }
	Iterator end() const	{ return Iterator(ENUM::Num); }
};
