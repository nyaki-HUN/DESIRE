#pragma once

#if defined(NEW_ARRAY)

template<typename T>
class Array
{
public:
	Array()
	{
	}

	Array(const Array& otherArray)
		: size(otherArray.size)
		, reservedSize(otherArray.reservedSize)
	{
		ASSERT(false);
		std::memcpy(m_data, otherArray.data, otherArray.size * sizeof(T));
	}

	Array(Array&& otherArray)
		: data(otherArray.data)
		, size(other.size)
		, reservedSize(other.reservedSize)
	{
		otherArray.data = nullptr;
		otherArray.size = 0;
		otherArray.reservedSize = 0;
	}

	Array(std::initializer_list<T> initList)
	{

	}

	Array& operator =(const Array& otherArray)
	{
		ASSERT(false);
		m_size = otherArray.m_size;
		m_reservedSize = otherArray.m_reservedSize;
		std::memcpy(m_data, otherArray.m_data, otherArray.m_size * sizeof(T));
		return *this;
	}

	Array& operator =(Array&& otherArray)
	{
		m_data = otherArray.m_data;
		m_size = otherArray.m_size;
		m_reservedSize = otherArray.m_reservedSize;

		otherArray.m_data = nullptr;
		otherArray.m_size = 0;
		otherArray.m_reservedSize = 0;

		return *this;
	}

	T& operator [](size_t idx)
	{
		ASSERT(idx < m_size);
		return m_data[idx];
	}
	const T& operator [](size_t idx) const
	{
		ASSERT(idx < m_size);
		return m_data[idx];
	}

	T& GetAt(size_t idx)
	{
		ASSERT(idx < m_size);
		return m_data[idx];
	}
	const T& GetAt(size_t idx) const
	{
		ASSERT(idx < m_size);
		return m_data[idx];
	}

	T& GetFirst()
	{
		ASSERT(m_size != 0);
		return m_data[0];
	}
	const T& GetFirst() const
	{
		ASSERT(m_size != 0);
		return m_data[0];
	}

	T& GetLast()
	{
		ASSERT(m_size != 0);
		return m_data[size - 1];
	}
	const T& GetLast() const
	{
		ASSERT(m_size != 0);
		return m_data[size - 1];
	}

	T* Data()
	{
		return m_data;
	}

	const T* Data() const
	{
		return m_data;
	}

	// Iterators for supporting range-based for loop
	T* begin()					{ return m_data; }
	const T* begin() const		{ return m_data; }
	T* end()					{ return m_data + m_size; }
	const T* end() const		{ return m_data + m_size; }

	bool IsEmpty() const
	{
		return (m_size == 0);
	}

	size_t Size() const
	{
		return m_size;
	}

	size_t SetSize(size_t newSize) const
	{
		ASSERT(false);
//		Reserve(newSize);
		m_size = newSize;
	}

	void Reserve(size_t newReservedSize)
	{
		ASSERT(false);
		m_reservedSize = newReservedSize;
	}

	// Erases all elements from the Array, but doesn't free its memory
	void Clear()
	{
		DestructElements(0, size);
		m_size = 0;
	}

	void Add(const T& value)
	{
		GrowIfNecessary();
		new (m_data + m_size) T(value);
		m_size++;
	}

	void Add(T&& value)
	{
		GrowIfNecessary();
		new (m_data + m_size) T(std::move(value));
		m_size++;
	}

	template<class... Args>
	T& EmplaceAdd(Args&&... args)
	{
		GrowIfNecessary();
		new (m_data + m_size) T(std::forward<Args>(args)...);
		m_size++;
		return m_data[m_size - 1];
	}

	void Insert(size_t pos, const T& value)
	{

	}

	void Insert(size_t pos, T&& value)
	{

	}

	size_t Find(const T& value) const
	{
		for(size_t i = 0; i < m_size; ++i)
		{
			if(m_data[i] == value)
			{
				return i;
			}
		}

		return SIZE_MAX;
	}

	size_t SpecializedFind(std::function<bool(const T&)> compareFunc) const
	{
		for(size_t i = 0; i < m_size; ++i)
		{
			if(compareFunc(m_data[i]))
			{
				return i;
			}
		}

		return SIZE_MAX;
	}

	bool Remove(const T& value)
	{
		const size_t idx = Find(value);
		if(idx != SIZE_MAX)
		{
			RemoveAt(idx);
			return true;
		}

		return false;
	}

	void RemoveAt(size_t idx)
	{
		ASSERT(idx < m_size);

		DestructElements(idx, 1);
		m_size--;
		std::memmove(m_data + idx, m_data + idx + 1, sizeof(T) * (m_size - idx));
	}

	void RemoveRangeAt(size_t idx, size_t count)
	{
		ASSERT(idx < m_size);

		count = std::min(count, m_size - idx);
		DestructElements(idx, count);
		std::memmove(m_data + idx, m_data + idx + count, sizeof(T) * (m_size - idx - count));
		m_size -= count;
	}

	// Removes an element by replacing it with the last element in the array and calling RemoveLast()
	// Note: This function does not preserve the order of elements
	bool RemoveFast(const T& value)
	{
		const size_t idx = Find(value);
		if(idx != SIZE_MAX)
		{
			RemoveFastAt(idx);
			return true;
		}

		return false;
	}

	void RemoveFastAt(size_t idx)
	{
		std::swap(GetAt(idx), GetLast());
		RemoveLast();
	}

	void RemoveLast()
	{
		if(size == 0)
		{
			return;
		}

		DestructElements(size - 1, 1);
		size--;
	}

	void Swap(Array& otherArray)
	{
		const T* tmpData = m_data;
		m_data = otherArray.m_data;
		otherArray.m_data = tmpData;

		const size_t tmpSize = m_size;
		m_size = otherArray.m_size;
		otherArray.m_size = tmpSize;

		const size_t tmpReservedSize = m_reservedSize;
		m_reservedSize = otherArray.m_reservedSize;
		otherArray.m_reservedSize = tmpReservedSize;
	}

private:
	static constexpr size_t kMaxReservedSizeIncrement = 1024;

	void GrowIfNecessary()
	{
		if(m_size == m_reservedSize)
		{
			Reserve(m_reservedSize < kMaxReservedSizeIncrement) ? (m_reservedSize << 1) : m_reservedSize + kMaxReservedSizeIncrement);
		}
	}

	void DestructElements(size_t fromIdx, size_t count)
	{
		T* pElements = m_data + fromIdx;
		for(size_t i = 0; i < count; ++i)
		{
			pElements[i].~T();
		}
	}

	T* m_data = nullptr;
	size_t m_size = 0;
	size_t m_reservedSize = 0;
};

#else

#include "Engine/Core/Container/ArrayAsVector.inl"

#endif
