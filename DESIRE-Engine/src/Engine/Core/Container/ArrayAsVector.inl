#include <vector>

template<typename T>
class Array
{
public:
	Array()
	{

	}

	Array(const Array& otherArray)
		: vector(otherArray.vector)
	{
	}

	Array(Array&& otherArray)
		: vector(std::move(otherArray.vector))
	{
	}

	Array(std::initializer_list<T> initList)
		: vector(initList)
	{
	}

	Array& operator =(const Array& otherArray)
	{
		vector = otherArray.vector;
		return *this;
	}

	Array& operator =(Array&& otherArray)
	{
		vector = std::move(otherArray.vector);
		return *this;
	}

	T& operator [](size_t idx)
	{
		return vector[idx];
	}
	const T& operator [](size_t idx) const
	{
		return vector[idx];
	}

	T& GetAt(size_t idx)
	{
		return vector[idx];
	}
	const T& GetAt(size_t idx) const
	{
		return vector[idx];
	}

	T& GetFirst()
	{
		return vector.front();
	}
	const T& GetFirst() const
	{
		return vector.front();
	}

	T& GetLast()
	{
		return vector.back();
	}
	const T& GetLast() const
	{
		return vector.back();
	}

	T* Data() const
	{
		return vector.data();
	}

	// Iterators for supporting range-based for loop
	T* begin()					{ return vector.data(); }
	const T* begin() const		{ return vector.data(); }
	T* end()					{ return vector.data() + vector.size(); }
	const T* end() const		{ return vector.data() + vector.size(); }

	bool IsEmpty() const
	{
		return vector.empty();
	}

	size_t Size() const
	{
		return vector.size();
	}

	void Reserve(size_t newReservedSize)
	{
		vector.reserve(newReservedSize);
	}

	// Erases all elements from the Array, but doesn't free any memory
	void Clear()
	{
		vector.clear();
	}

	void Add(const T& value)
	{
		vector.push_back(value);
	}

	void Add(T&& value)
	{
		vector.push_back(std::move(value));
	}

	template<class... Args>
	T& EmplaceAdd(Args&&... args)
	{
		return vector.emplace_back(std::forward<Args>(args)...);
	}

	void Insert(size_t pos, const T& value)
	{
		vector.insert(value);
	}

	void Insert(size_t pos, T&& value)
	{
		vector.insert(std::move(value));
	}

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
			RemoveAt(idx);
			return true;
		}

		return false;
	}

	void RemoveAt(size_t idx)
	{
		vector.erase(vector.begin() + idx);
	}

	void RemoveRangeAt(size_t idx, size_t count)
	{
		ASSERT(idx < vector.size());

		count = std::min(count, vector.size() - idx);
		vector.erase(vector.begin() + idx, vector.begin() + idx + count);
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
		if(vector.size() == 0)
		{
			return;
		}

		vector.pop_back();
	}

	void Swap(Array& otherArray)
	{
		vector.swap(otherArray.vector);
	}

private:
	std::vector<T> vector;
};
