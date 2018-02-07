#pragma once

#include <vector>
#include <algorithm>

namespace stl_utils
{

// --------------------------------------------------------------------------------------------------------------------
// Do a binary search on a vector to find an element by value and return an iterator to it.
// --------------------------------------------------------------------------------------------------------------------
template<typename T, typename A>
inline auto binary_find(std::vector<T, A>& container, const T& val)
{
	auto it = std::lower_bound(container.begin(), container.end(), val);
	return (it != container.end() && !(val < *it)) ? it : container.end();
}

template<typename T, typename A>
inline const auto binary_find(const std::vector<T, A>& container, const T& val)
{
	auto it = std::lower_bound(container.begin(), container.end(), val);
	return (it != container.end() && !(val < *it)) ? it : container.end();
}

// --------------------------------------------------------------------------------------------------------------------
// Do a binary search on a vector to find an element by id and return an iterator to it.
// !!! WARNING: There must be a 'T2 id' as the first member variable in T1 type in order to this work properly !!!
// --------------------------------------------------------------------------------------------------------------------
template<typename T1, typename A, typename T2>
inline auto binary_find_by_id(std::vector<T1, A>& container, const T2& id)
{
	auto it = std::lower_bound(container.begin(), container.end(), id, [](const T1& a, const T2& id)
	{
		return *reinterpret_cast<const T2*>(&a) < id;
	});
	return (it != container.end() && !(id < *reinterpret_cast<const T2*>(&*it))) ? it : container.end();
}

template<typename T1, typename A, typename T2>
inline const auto binary_find_by_id(const std::vector<T1, A>& container, const T2& id)
{
	auto it = std::lower_bound(container.begin(), container.end(), id, [](const T1& a, const T2& id)
	{
		return *reinterpret_cast<const T2*>(&a) < id;
	});
	return (it != container.end() && !(id < *reinterpret_cast<const T2*>(&*it))) ? it : container.end();
}

// --------------------------------------------------------------------------------------------------------------------
// Do a binary search on a vector to find an element by value and return an iterator to it.
// If the element is not found, 'val' will be inserted in a way that the vector stays sorted.
// --------------------------------------------------------------------------------------------------------------------
template<typename T, typename A>
inline auto binary_find_or_insert(std::vector<T, A>& container, T&& val)
{
	auto it = std::lower_bound(container.begin(), container.end(), val);
	return (it != container.end() && !(val < *it)) ? it : container.insert(it, std::move(val));
}

// Variant with compare func
template<typename T, typename A>
inline auto binary_find_or_insert(std::vector<T, A>& container, T&& val, bool(*compareFunc)(const T&, const T&))
{
	auto it = std::lower_bound(container.begin(), container.end(), val, compareFunc);
	return (it != container.end() && !compareFunc(val, *it)) ? it : container.insert(it, std::move(val));
}

// --------------------------------------------------------------------------------------------------------------------
// Do a binary search on a vector to find an element by id and return an iterator to it.
// If the element is not found, a new element (constructed from the id) will be inserted in a way that the vector stays sorted.
// !!! WARNING: There must be a 'T2 id' as the first member variable in T1 type in order to this work properly !!!
// --------------------------------------------------------------------------------------------------------------------
template<typename T1, typename A, typename T2>
inline auto binary_find_or_insert_by_id(std::vector<T1, A>& container, const T2& id)
{
	auto it = std::lower_bound(container.begin(), container.end(), id, [](const T1& a, const T2& id)
	{
		return *reinterpret_cast<const T2*>(&a) < id;
	});
	return (it != container.end() && !(id < *reinterpret_cast<const T2*>(&*it))) ? it : container.insert(it, { id });
}

// --------------------------------------------------------------------------------------------------------------------
// Generic hashing function for std::pair
// An std::hash<> implementation must be exist for both T1 and T2
// --------------------------------------------------------------------------------------------------------------------
template<typename T1, typename T2>
struct hash_pair
{
	std::size_t operator()(const std::pair<T1, T2>& pair) const
	{
		return std::hash<T1>{}(pair.first) ^ std::hash<T2>{}(pair.second);
	}
};

}	// end of namespace stl_utils
