#pragma once

#include "Engine/Core/assert.h"
#include "Engine/Core/HashedString.h"
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
		ASSERT(Find(key) == nullptr && "An other value is already added with this key");
		elements.BinaryFindOrInsert(KeyValuePair(key, value));
	}

	T* Find(HashedString key)
	{
		// Binary find
		auto it = std::lower_bound(elements.begin(), elements.end(), key, [](const KeyValuePair& pair, HashedString key)
		{
			return (pair.key < key);
		});
		return (it != elements.end() && !(key < it->key)) ? &it->value : nullptr;
	}

	const T* Find(HashedString key) const
	{
		// Binary find
		auto it = std::lower_bound(elements.begin(), elements.end(), key, [](const KeyValuePair& pair, HashedString key)
		{
			return (pair.key < key);
		});
		return (it != elements.end() && !(key < it->key)) ? &it->value : nullptr;
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

		bool operator <(const KeyValuePair& other) const
		{
			return (key < other.key);
		}
	};

	Array<KeyValuePair> elements;
};
