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
		const size_t idx = m_elements.BinaryFind(key);
		return (idx != SIZE_MAX) ? &m_elements[idx].value : nullptr;
	}

	const T* Find(HashedString key) const
	{
		const size_t idx = m_elements.BinaryFind(key);
		return (idx != SIZE_MAX) ? &m_elements[idx].value : nullptr;
	}

	bool IsEmpty() const
	{
		return m_elements.IsEmpty();
	}

	size_t Size() const
	{
		return m_elements.Size();
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

		bool operator <(const KeyValuePair& other) const		{ return (key < other.key); }
		bool operator ==(const KeyValuePair& other) const		{ return (key == other.key); }

		bool operator <(const HashedString& otherKey) const		{ return (key < otherKey); }
		bool operator ==(const HashedString& otherKey) const	{ return (key == otherKey); }
	};

	Array<KeyValuePair> m_elements;
};
