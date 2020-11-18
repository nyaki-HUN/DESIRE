#pragma once

#include "Engine/Core/Container/Array.h"
#include "Engine/Core/String/HashedString.h"

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
		m_elements.Swap(otherMap.m_elements);
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
		m_elements.Swap(otherMap.m_elements);
		return *this;
	}

	T& Insert(HashedString key, const T& value)
	{
		ASSERT(Find(key) == nullptr && "An other value is already added with this key");
		return m_elements.BinaryFindOrInsert(KeyValuePair(key, value)).value;
	}

	T* Find(HashedString key)
	{
		// Note: This is a custom variant of m_elements.BinaryFind() to compare KeyValuePair with HashedString
		auto it = std::lower_bound(m_elements.begin(), m_elements.end(), key, [](const KeyValuePair& pair, HashedString key)
		{
			return (pair.key < key);
		});
		return (it != m_elements.end() && !(key < it->key)) ? &it->value : nullptr;
	}

	const T* Find(HashedString key) const
	{
		// Note: This is a custom variant of m_elements.BinaryFind() to compare KeyValuePair with HashedString
		auto it = std::lower_bound(m_elements.begin(), m_elements.end(), key, [](const KeyValuePair& pair, HashedString key)
		{
			return (pair.key < key);
		});
		return (it != m_elements.end() && !(key < it->key)) ? &it->value : nullptr;
	}

	void Clear()
	{
		m_elements.Clear();
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

	Array<KeyValuePair> m_elements;
};
