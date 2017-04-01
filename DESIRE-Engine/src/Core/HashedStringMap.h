#pragma once

#include "Core/Assert.h"
#include "Core/HashedString.h"

#include <vector>

// --------------------------------------------------------------------------------------------------------------------
//	HashedStringMap is a sorted associative container that contains key-value pairs with unique HashedString keys.
//  Note: In case of a collision, the element is dropped
// --------------------------------------------------------------------------------------------------------------------

template<typename T>
class HashedStringMap
{
public:
	HashedStringMap()
	{

	}

	HashedStringMap(HashedStringMap&& otherMap)
	{
		elements.swap(otherMap.elements);
	}

	HashedStringMap& operator =(HashedStringMap&& otherMap)
	{
		elements.swap(otherMap.elements);
		return *this;
	}

	bool Insert(HashedString key, const T& value)
	{
		size_t idx;
		if(FindKeyIndex(key, idx))
		{
			ASSERT(false && "Element already added with this key");
			return false;
		}

		elements.emplace(std::next(elements.begin(), (ptrdiff_t)idx), key, value);
		return true;
	}

	T* Find(HashedString key)
	{
		size_t idx;
		return (FindKeyIndex(key, idx) ? &elements[idx].second : nullptr);
	}

	const T* Find(HashedString key) const
	{
		size_t idx;
		return (FindKeyIndex(key, idx) ? &elements[idx].second : nullptr);
	}

	// Remove all elements from the map
	void Clear()
	{
		elements.clear();
	}

private:
	bool FindKeyIndex(HashedString key, size_t& idx) const
	{
		// Do binary-search on elements
		size_t left = 0;
		size_t right = elements.size();
		size_t middle = 0;
		while(left < right)
		{
			middle = left + (right - left) / 2;
			const auto& pair = elements[middle];
			if(key > pair.first)
			{
				left = middle + 1;
			}
			else if(key < pair.first)
			{
				right = middle;
			}
			else
			{
				idx = middle;
				return true;
			}
		}

		idx = right;
		return false;
	}

	std::vector<std::pair<HashedString, T>> elements;
};
