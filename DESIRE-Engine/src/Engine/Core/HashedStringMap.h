#pragma once

#include "Engine/Core/assert.h"
#include "Engine/Core/HashedString.h"
#include "Engine/Core/STL_utils.h"
#include "Engine/Core/Container/Array.h"

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
		elements.Swap(otherMap.elements);
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
		elements.Swap(otherMap.elements);
		return *this;
	}

	void Insert(HashedString key, const T& value)
	{
		KeyValuePair& pair = elements.BinaryFindOrInsert(KeyValuePair(key, value));
		ASSERT(memcmp(&pair.value, &value, sizeof(T)) == 0 && "An other value is already added with this key");
	}

	T* Find(HashedString key)
	{
		auto it = stl_utils::binary_find_by_id(elements, key);
		return (it != elements.end()) ? &it->value : nullptr;
	}

	const T* Find(HashedString key) const
	{
		auto it = stl_utils::binary_find_by_id(elements, key);
		return (it != elements.end()) ? &it->value : nullptr;
	}

	// Remove all elements from the map
	void Clear()
	{
		elements.Clear();
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

	Array<KeyValuePair> elements;
};
