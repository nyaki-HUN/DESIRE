#pragma once

#include "Engine/Core/assert.h"

#include <functional>

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
		memcpy(data, otherArray.data, otherArray.size * sizeof(T));
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
		size = otherArray.size;
		reservedSize = otherArray.reservedSize;
		memcpy(data, otherArray.data, otherArray.size * sizeof(T));
		return *this;
	}

	Array& operator =(Array&& otherArray)
	{
		data = otherArray.data;
		size = otherArray.size;
		reservedSize = otherArray.reservedSize;

		otherArray.data = nullptr;
		otherArray.size = 0;
		otherArray.reservedSize = 0;

		return *this;
	}

	T& operator [](size_t idx)
	{
		ASSERT(idx < size);
		return data[idx];
	}
	const T& operator [](size_t idx) const
	{
		ASSERT(idx < size);
		return data[idx];
	}

	T& GetAt(size_t idx)
	{
		ASSERT(idx < size);
		return data[idx];
	}
	const T& GetAt(size_t idx) const
	{
		ASSERT(idx < size);
		return data[idx];
	}

	T& GetFirst()
	{
		ASSERT(size != 0);
		return data[0];
	}
	const T& GetFirst() const
	{
		ASSERT(size != 0);
		return data[0];
	}

	T& GetLast()
	{
		ASSERT(size != 0);
		return data[size - 1];
	}
	const T& GetLast() const
	{
		ASSERT(size != 0);
		return data[size - 1];
	}

	T* Data() const
	{
		return data;
	}

	// Iterators for supporting range-based for loop
	T* begin()					{ return data; }
	const T* begin() const		{ return data; }
	T* end()					{ return data + size; }
	const T* end() const		{ return data + size; }

	bool IsEmpty() const
	{
		return (size == 0);
	}

	size_t Size() const
	{
		return size;
	}

	void Reserve(size_t newReservedSize)
	{
		reservedSize = newReservedSize;
	}

	// Erases all elements from the Array, but doesn't free its memory
	void Clear()
	{
		DestructElements(0, size);
		size = 0;
	}

	void Add(const T& value)
	{
		GrowIfNecessary();
		new (data + size) T(value);
		size++;
	}

	void Add(T&& value)
	{
		GrowIfNecessary();
		new (data + size) T(std::move(value));
		size++;
	}

	template<class... Args>
	T& EmplaceAdd(Args&&... args)
	{
		GrowIfNecessary();
		new (data + size) T(std::forward<Args>(args)...);
		size++;
		return data[size - 1];
	}

	void Insert(size_t pos, const T& value)
	{

	}

	void Insert(size_t pos, T&& value)
	{

	}

	size_t Find(const T& value) const
	{
		for(size_t i = 0; i < size; ++i)
		{
			if(data[i] == value)
			{
				return i;
			}
		}

		return SIZE_MAX;
	}

	size_t SpecializedFind(std::function<bool(const T&)> compareFunc) const
	{
		for(size_t i = 0; i < size; ++i)
		{
			if(compareFunc(data[i]))
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
		ASSERT(idx < size);

		DestructElements(idx, 1);
		size--;
		memmove(data + idx, data + idx + 1, size - idx);
	}

	void RemoveRangeAt(size_t idx, size_t count)
	{
		ASSERT(idx < size);

		count = std::min(count, size - idx);
		DestructElements(idx, count);
		memmove(data + idx, data + idx + count, size - idx - count);
		size -= count;
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
		const T *tmpData = data;
		data = otherArray.data;
		otherArray.data = tmpData;

		const size_t tmpSize = size;
		size = otherArray.size;
		otherArray.size = tmpSize;

		const size_t tmpReservedSize = reservedSize;
		reservedSize = otherArray.reservedSize;
		otherArray.reservedSize = tmpReservedSize;
	}

private:
	static constexpr size_t kMaxReservedSizeIncrement = 1024;

	void GrowIfNecessary()
	{
		if(size == reservedSize)
		{
			Reserve(reservedSize < kMaxReservedSizeIncrement) ? (reservedSize << 1) : reservedSize + kMaxReservedSizeIncrement);
		}
	}

	void DestructElements(size_t fromIdx, size_t count)
	{
		T *elements = data + fromIdx;
		for(size_t i = 0; i < count; ++i)
		{
			elements[i].~T();
		}
	}

	T *data = nullptr;
	size_t size = 0;
	size_t reservedSize = 0;
};

#else

#include "Engine/Core/Container/ArrayAsVector.inl"

#endif
