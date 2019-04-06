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
	{

	}

	Array(Array&& otherArray)
	{

	}

	Array(std::initializer_list<T> initList)
	{

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
		return vector[idx];
	}
	const T& GetAt(size_t idx) const
	{
		ASSERT(idx < size);
		return vector[idx];
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
	T* begin()					{ return &data[0]; }
	const T* begin() const		{ return &data[0]; }
	T* end()					{ return &data[size]; }
	const T* end() const		{ return &data[size]; }

	bool IsEmpty() const
	{
		return (size == 0);
	}

	size_t Size() const
	{
		return size;
	}

	void Reserve(size_t numElements)
	{
		reservedSize = numElements;
	}

	// Erases all elements from the Array, but doesn't free any memory
	void Clear()
	{

	}

	void Add(const T& value)
	{
		
	}

	void Add(T&& value)
	{

	}

	template<class... Args>
	T& EmplaceAdd(Args&&... args)
	{
		new (&data[0]) T(std::forward<Args>(args)...);
		return data[0];
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
			Remove(idx);
			return true;
		}

		return false;
	}

	void Remove(size_t idx)
	{
		ASSERT(idx < size);
		memmove(&data[idx], data + idx + 1, size - idx);
	}

	// Removes an element by replacing it with the last element in the array and calling RemoveLast()
	// Note: This function does not preserve the order of elements
	void RemoveFast(size_t idx)
	{
		ASSERT(idx < size);
		std::swap(data[idx], data + idx + 1, size - idx);
		RemoveLast();
	}

	void RemoveLast()
	{

	}

	void Swap(Array& other)
	{
		const T *tmpData = data;
		data = other.data;
		other.data = tmpData;

		const size_t tmpSize = size;
		size = other.size;
		other.size = tmpSize;

		const size_t tmpReservedSize = reservedSize;
		reservedSize = other.reservedSize;
		other.reservedSize = tmpReservedSize;
	}

private:
	T *data = nullptr;
	size_t size = 0;
	size_t numPreallocatedElements = 0;
};

#else

#include <vector>

template<typename T>
class Array
{
public:
	Array()											{}
	Array(const Array& otherArray)					: vector(otherArray.vector) {}
	Array(Array&& otherArray)						: vector(std::move(otherArray.vector)) {}
	Array(std::initializer_list<T> initList)		: vector(initList) {}

	Array& operator =(const Array& otherArray)		{ vector = otherArray.vector; return *this; }
	Array& operator =(Array&& otherArray)			{ vector = std::move(otherArray.vector); return *this; }

	T& operator [](size_t idx)						{ return vector[idx]; }
	const T& operator [](size_t idx) const			{ return vector[idx]; }
	T& GetAt(size_t idx)							{ return vector.at(idx); }
	const T& GetAt(size_t idx) const				{ return vector.at(idx); }
	T& GetFirst()									{ return vector.front(); }
	const T& GetFirst() const						{ return vector.front(); }
	T& GetLast()									{ return vector.back(); }
	const T& GetLast() const						{ return vector.back(); }

	T* Data() const									{ return vector.data(); }

	// Iterators for supporting range-based for loop
	T* begin()										{ return &(*vector.begin()); }
	const T* begin() const							{ return &(*vector.begin()); }
	T* end()										{ return &(*vector.end()); }
	const T* end() const							{ return &(*vector.end()); }

	bool IsEmpty() const							{ return vector.empty(); }
	size_t Size() const								{ return vector.size(); }
	void Reserve(size_t numElements)				{ vector.reserve(numElements); }
	void Clear()									{ vector.clear(); }

	void Add(const T& value)						{ vector.push_back(value); }
	void Add(T&& value)								{ vector.push_back(std::move(value)); }

	template<class... Args>
	T& EmplaceAdd(Args&&... args)					{ return vector.emplace_back(std::forward<Args>(args)...); }

	void Insert(size_t pos, const T& value)			{ vector.insert(value); }
	void Insert(size_t pos, T&& value)				{ vector.insert(std::move(value)); }

	size_t Find(const T& value) const
	{
		for(size_t i = 0; i < vector.size(); ++i)
		{
			if(vector[i] == value)
			{
				return i;
			}
		}

		return SIZE_MAX;
	}

	size_t SpecializedFind(std::function<bool(const T&)> compareFunc) const
	{
		for(size_t i = 0; i < vector.size(); ++i)
		{
			if(compareFunc(vector[i]))
			{
				return i;
			}
		}

		return SIZE_MAX;
	}

	// Do a binary search to find the index of a given element
	size_t BinaryFind(const T& value) const
	{
		auto it = std::lower_bound(vector.begin(), vector.end(), value);
		return (it != vector.end() && !(value < *it)) ? (it - vector.begin()) : SIZE_MAX;
	}

	// Do a binary search to find an element by value or insert it into a sorted array 
	T& BinaryFindOrInsert(T&& value)
	{
		auto it = std::lower_bound(vector.begin(), vector.end(), value);
		return (it != vector.end() && !(value < *it)) ? *it : *vector.insert(it, std::move(value));
	}

	T& BinaryFindOrInsert(T&& value, bool(*compareFunc)(const T&, const T&))
	{
		auto it = std::lower_bound(vector.begin(), vector.end(), value, compareFunc);
		return (it != vector.end() && !compareFunc(value, *it)) ? *it : *vector.insert(it, std::move(value));
	}

	bool Remove(const T& value)
	{
		const size_t idx = Find(value);
		if(idx != SIZE_MAX)
		{
			Remove(idx);
			return true;
		}

		return false;
	}

	void Remove(size_t idx)							{ vector.erase(vector.begin() + idx); }
	void RemoveLast()								{ vector.pop_back(); }

	void Swap(Array& other)							{ vector.swap(other.vector); }

private:
	std::vector<T> vector;
};

#endif
