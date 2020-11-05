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

	T& operator [](size_t idx)
	{
		return m_vector[idx];
	}
	const T& operator [](size_t idx) const
	{
		return m_vector[idx];
	}

	T& GetAt(size_t idx)
	{
		return m_vector[idx];
	}
	const T& GetAt(size_t idx) const
	{
		return m_vector[idx];
	}

	T& GetFirst()
	{
		return m_vector.front();
	}
	const T& GetFirst() const
	{
		return m_vector.front();
	}

	T& GetLast()
	{
		return m_vector.back();
	}
	const T& GetLast() const
	{
		return m_vector.back();
	}

	T* Data()
	{
		return m_vector.data();
	}

	const T* Data() const
	{
		return m_vector.data();
	}

	// Iterators for supporting range-based for loop
	T* begin()					{ return m_vector.data(); }
	const T* begin() const		{ return m_vector.data(); }
	T* end()					{ return m_vector.data() + m_vector.size(); }
	const T* end() const		{ return m_vector.data() + m_vector.size(); }

	bool IsEmpty() const
	{
		return m_vector.empty();
	}

	size_t Size() const
	{
		return m_vector.size();
	}

	void SetSize(size_t newSize)
	{
		m_vector.resize(newSize);
	}

	void Reserve(size_t newReservedSize)
	{
		m_vector.reserve(newReservedSize);
	}

	// Erases all elements from the Array, but doesn't free any memory
	void Clear()
	{
		m_vector.clear();
	}

	void Add(const T& value)
	{
		m_vector.push_back(value);
	}

	void Add(T&& value)
	{
		m_vector.push_back(std::move(value));
	}

	template<class... Args>
	T& EmplaceAdd(Args&&... args)
	{
		return m_vector.emplace_back(std::forward<Args>(args)...);
	}

	void Insert(size_t pos, const T& value)
	{
		m_vector.insert(pos, value);
	}

	void Insert(size_t pos, T&& value)
	{
		m_vector.insert(pos, std::move(value));
	}

	size_t Find(const T& value) const
	{
		for(size_t i = 0; i < m_vector.size(); ++i)
		{
			if(m_vector[i] == value)
			{
				return i;
			}
		}

		return SIZE_MAX;
	}

	size_t SpecializedFind(std::function<bool(const T&)> compareFunc) const
	{
		for(size_t i = 0; i < m_vector.size(); ++i)
		{
			if(compareFunc(m_vector[i]))
			{
				return i;
			}
		}

		return SIZE_MAX;
	}

	// Do a binary search to find the index of a given element
	size_t BinaryFind(const T& value) const
	{
		auto it = std::lower_bound(m_vector.begin(), m_vector.end(), value);
		return (it != m_vector.end() && !(value < *it)) ? (it - m_vector.begin()) : SIZE_MAX;
	}

	// Do a binary search to find an element by value or insert it into a sorted array 
	T& BinaryFindOrInsert(T&& value)
	{
		auto it = std::lower_bound(m_vector.begin(), m_vector.end(), value);
		return (it != m_vector.end() && !(value < *it)) ? *it : *m_vector.insert(it, std::move(value));
	}

	T& BinaryFindOrInsert(T&& value, bool(*compareFunc)(const T&, const T&))
	{
		auto it = std::lower_bound(m_vector.begin(), m_vector.end(), value, compareFunc);
		return (it != m_vector.end() && !compareFunc(value, *it)) ? *it : *m_vector.insert(it, std::move(value));
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
		ASSERT(idx < m_vector.size());

		count = std::min(count, m_vector.size() - idx);
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
		if(m_vector.size() == 0)
		{
			return;
		}

		m_vector.pop_back();
	}

	void Swap(Array& otherArray)
	{
		m_vector.swap(otherArray.m_vector);
	}

private:
	std::vector<T> m_vector;
};
