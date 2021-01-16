#pragma once

#include <vector>

template<typename T>
class Array
{
public:
	Array()
	{
	}

	Array(const Array& otherArray)
		: m_vector(otherArray.m_vector)
	{
	}

	Array(Array&& otherArray)
		: m_vector(std::move(otherArray.m_vector))
	{
	}

	Array(std::initializer_list<T> initList)
		: m_vector(initList)
	{
	}

	Array& operator =(const Array& otherArray)
	{
		m_vector = otherArray.m_vector;
		return *this;
	}

	Array& operator =(Array&& otherArray)
	{
		m_vector = std::move(otherArray.m_vector);
		return *this;
	}

	// Element access
	T& operator [](size_t idx)					{ return m_vector[idx]; }
	const T& operator [](size_t idx) const		{ return m_vector[idx]; }

	T& GetAt(size_t idx)						{ return m_vector[idx]; }
	const T& GetAt(size_t idx) const			{ return m_vector[idx]; }

	T& GetFirst()								{ return m_vector.front(); }
	const T& GetFirst() const					{ return m_vector.front(); }

	T& GetLast()								{ return m_vector.back(); }
	const T& GetLast() const					{ return m_vector.back(); }

	T* Data()									{ return m_vector.data(); }
	const T* Data() const						{ return m_vector.data(); }

	// Iterators for supporting range-based for loop
	T* begin()									{ return Data(); }
	const T* begin() const						{ return Data(); }
	T* end()									{ return Data() + Size(); }
	const T* end() const						{ return Data() + Size(); }

	// Capacity
	bool IsEmpty() const						{ return m_vector.empty(); }
	size_t Size() const							{ return m_vector.size(); }
	void SetSize(size_t newSize)				{ m_vector.resize(newSize); }
	size_t GetReservedSize() const				{ return m_vector.capacity(); }
	void Reserve(size_t newReservedSize)		{ m_vector.reserve(newReservedSize); }

	// Erases all elements from the Array, but doesn't free its memory
	void Clear()
	{
		m_vector.clear();
	}

	void Add(const T& value)
	{
		GrowIfNecessary();
		m_vector.push_back(value);
	}

	void Add(T&& value)
	{
		GrowIfNecessary();
		m_vector.push_back(std::move(value));
	}

	template<class... Args>
	T& EmplaceAdd(Args&&... args)
	{
		GrowIfNecessary();
		return m_vector.emplace_back(std::forward<Args>(args)...);
	}

	T& Insert(size_t pos, const T& value)
	{
		GrowIfNecessary();
		return *m_vector.insert(m_vector.begin() + pos, value);
	}

	T& Insert(size_t pos, T&& value)
	{
		GrowIfNecessary();
		return *m_vector.insert(m_vector.begin() + pos, std::move(value));
	}

	size_t Find(const T& value) const
	{
		const size_t size = Size();
		for(size_t i = 0; i < size; ++i)
		{
			if(GetAt(i) == value)
			{
				return i;
			}
		}

		return SIZE_MAX;
	}

	size_t SpecializedFind(std::function<bool(const T&)> compareFunc) const
	{
		const size_t size = Size();
		for(size_t i = 0; i < size; ++i)
		{
			if(compareFunc(GetAt(i)))
			{
				return i;
			}
		}

		return SIZE_MAX;
	}

	// Do a binary search to find the index of a given element. The type U has to be comparable to type T.
	template<class U>
	size_t BinaryFind(const U& value) const
	{
		auto iter = std::lower_bound(m_vector.begin(), m_vector.end(), value);
		return (iter != m_vector.end() && *iter == value) ? (iter - m_vector.begin()) : SIZE_MAX;
	}

	// Do a binary search to find an element by value or insert it into a sorted array
	T& BinaryFindOrInsert(T&& value)
	{
		auto iter = std::lower_bound(m_vector.begin(), m_vector.end(), value);
		return (iter != m_vector.end() && *iter == value) ? *iter : Insert(iter - m_vector.begin(), std::move(value));
	}

	T& BinaryFindOrInsert(T&& value, bool(*compareFunc)(const T&, const T&))
	{
		auto iter = std::lower_bound(m_vector.begin(), m_vector.end(), value, compareFunc);
		return (iter != m_vector.end() && !compareFunc(value, *iter)) ? *iter : Insert(iter - m_vector.begin(), std::move(value));
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
		m_vector.erase(m_vector.begin() + idx);
	}

	void RemoveRangeAt(size_t idx, size_t count)
	{
		ASSERT(idx < Size());

		count = std::min(count, Size() - idx);
		m_vector.erase(m_vector.begin() + idx, m_vector.begin() + idx + count);
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
		if(!IsEmpty())
		{
			m_vector.pop_back();
		}
	}

	void Swap(Array& otherArray)
	{
		m_vector.swap(otherArray.m_vector);
	}

private:
	static constexpr size_t kMaxReservedSizeIncrement = 1024;

	void GrowIfNecessary()
	{
		const size_t reservedSize = GetReservedSize();
		if(Size() == reservedSize)
		{
			const size_t newReservedSize = (reservedSize < kMaxReservedSizeIncrement) ? (reservedSize << 1) : (reservedSize + kMaxReservedSizeIncrement);
			Reserve(newReservedSize);
		}
	}

	std::vector<T> m_vector;
};
