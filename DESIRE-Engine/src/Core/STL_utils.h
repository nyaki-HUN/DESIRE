#pragma once

#include <vector>
#include <algorithm>
#include <memory>		// for std::weak_ptr

namespace stl_utils
{

// Do a binary search on a vector to find an element and return an iterator to it.
// Note: When T1 is different from T2 the comparision is done in a way as if T2 would be the first member variable of T1.
//	This is useful for cases when we want to find a struct by an id where the id is the first member of the struct.
template<typename T1, typename A, typename T2>
inline auto binary_find(std::vector<T1, A>& container, const T2& val)
{
	auto it = std::lower_bound(container.begin(), container.end(), val, [](const T1& a, const T2& b)
	{
		return *reinterpret_cast<const T2*>(&a) < b;
	});
	return (it != container.end() && !(val < *reinterpret_cast<const T2*>(&*it))) ? it : container.end();
}

template<typename T1, typename A, typename T2>
inline const auto binary_find(const std::vector<T1, A>& container, const T2& val)
{
	auto it = std::lower_bound(container.begin(), container.end(), val, [](const T1& a, const T2& b)
	{
		return *reinterpret_cast<const T2*>(&a) < b;
	});
	return (it != container.end() && !(val < *reinterpret_cast<const T2*>(&*it))) ? it : container.end();
}

// Do a binary search on a vector to find an element and return an iterator to it.
// If the element is not found, 'val' will be inserted in a way that the vector stays sorted.
// Note: T1 must be constructible from T2.
template<typename T1, typename A, typename T2>
inline auto binary_find_or_insert(std::vector<T1, A>& container, const T2& val)
{
	auto it = std::lower_bound(container.begin(), container.end(), val, [](const T1& a, const T2& b)
	{
		return *reinterpret_cast<const T2*>(&a) < b;
	});
	return (it != container.end() && !(val < *reinterpret_cast<const T2*>(&*it))) ? it : container.insert(it, val);
}

}	// end of namespace stl_utils

// Checks whether weak_ptr 'a' equals to 'b'
template<typename T>
inline bool operator ==(const std::weak_ptr<T>& a, const std::weak_ptr<T>& b)
{
	return !a.owner_before(b) && !b.owner_before(a);
}
