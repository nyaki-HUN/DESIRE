#pragma once

#include <vector>
#include <algorithm>

namespace stl_utils
{

// --------------------------------------------------------------------------------------------------------------------
// Generic hashing function for std::pair
// An std::hash<> implementation must be exist for both T1 and T2
// --------------------------------------------------------------------------------------------------------------------
template<typename T1, typename T2>
struct hash_pair
{
	size_t operator()(const std::pair<T1, T2>& pair) const
	{
		return std::hash<T1>{}(pair.first) ^ std::hash<T2>{}(pair.second);
	}
};

}	// end of namespace stl_utils
