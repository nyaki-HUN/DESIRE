#pragma once

#include <vector>
#include <algorithm>

namespace core
{

template<class T, class A>
inline const T* binary_search(const std::vector<T, A>& container, const T& val)
{
	auto it = std::lower_bound(container.begin(), container.end(), val);
	if(it != container.end() && !(val < *it))
	{
		return &(*it);
	}

	return nullptr;
}

template<class T, class A>
inline T& binary_search_or_insert(std::vector<T, A>& container, T&& val)
{
	auto it = std::lower_bound(container.begin(), container.end(), val);
	if(it != container.end() && !(val < *it))
	{
		return (*it);
	}

	it = container.insert(it, std::move(val));
	return (*it);
}

}	// end of namespace core
