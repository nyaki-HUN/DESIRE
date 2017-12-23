#pragma once

#include "Core/Assert.h"
#include "Core/HashedString.h"
#include "Core/STL_utils.h"

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

	HashedStringMap(std::initializer_list<std::pair<HashedString, T>> initList)
	{
		for(const std::pair<HashedString, T>& pair : initList)
		{
			Insert(pair.first, pair.second);
		}
	}

	HashedStringMap& operator =(HashedStringMap&& otherMap)
	{
		elements.swap(otherMap.elements);
		return *this;
	}

	void Insert(HashedString key, const T& value)
	{
		auto it = stl_utils::binary_find_or_insert(elements, KeyValuePair(key, value));
		ASSERT(it->value == value && "An other value is already added with this key");
	}

	T* Find(HashedString key)
	{
		auto it = stl_utils::binary_find(elements, key);
		return (it != elements.end()) ? &it->value : nullptr;
	}

	const T* Find(HashedString key) const
	{
		auto it = stl_utils::binary_find(elements, key);
		return (it != elements.end()) ? &it->value : nullptr;
	}

	// Remove all elements from the map
	void Clear()
	{
		elements.clear();
	}

private:
	struct KeyValuePair
	{
		HashedString key;
		T value;

		KeyValuePair(HashedString key, const T& value)
			: key(key)
			, value(value)
		{

		}

		inline bool operator <(const KeyValuePair& other) const
		{
			return key < other.key;
		}
	};

	std::vector<KeyValuePair> elements;
};
