#pragma once

// --------------------------------------------------------------------------------------------------------------------
//	LockFreeRingBuffer is a one producer and one consumer queue without locks.
//	Note that the number of element slots in the buffer is actually (SIZE - 1).
// --------------------------------------------------------------------------------------------------------------------

template<typename T, uint32_t SIZE>
class LockFreeRingBuffer
{
public:
	LockFreeRingBuffer()
		: m_readIdx(0)
		, m_writeIdx(0)
	{
		static_assert(SIZE >= 2);
	}

	// No synchronization needed in destructor as only one thread can do this
	~LockFreeRingBuffer() {}

	// Add new element at the end of the buffer, after its current last element
	// The content of value is copied to the new element
	bool push(const T& value)
	{
		const uint32_t currWrite = m_writeIdx.load(std::memory_order_relaxed);
		const uint32_t nextIdx = (currWrite + 1) % SIZE;
		if(nextIdx == m_readIdx.load(std::memory_order_acquire))
		{
			// The buffer is full
			return false;
		}

		m_data[currWrite] = value;
		m_writeIdx.store(nextIdx, std::memory_order_release);
		return true;
	}

	// Add new element at the end of the buffer, after its current last element
	// The content of value is moved to the new element
	bool push(T&& value)
	{
		const uint32_t currWrite = m_writeIdx.load(std::memory_order_relaxed);
		const uint32_t nextIdx = (currWrite + 1) % SIZE;
		if(nextIdx == m_readIdx.load(std::memory_order_acquire))
		{
			// The buffer is full
			return false;
		}

		m_data[currWrite] = std::move(value);
		m_writeIdx.store(nextIdx, std::memory_order_release);
		return true;
	}

	// Remove the next element in the buffer by moving it to the given variable
	bool pop(T& value)
	{
		const uint32_t currRead = m_readIdx.load(std::memory_order_relaxed);
		if(currRead == m_writeIdx.load(std::memory_order_acquire))
		{
			// The buffer is empty
			return false;
		}

		const uint32_t nextIdx = (currRead + 1) % SIZE;
		value = std::move(m_data[currRead]);
		m_readIdx.store(nextIdx, std::memory_order_release);
		return true;
	}

	// Returns true if there is no element in the buffer
	bool empty() const
	{
		return m_readIdx.load(std::memory_order_consume) == m_writeIdx.load(std::memory_order_consume);
	}

private:
	DESIRE_NO_COPY_AND_MOVE(LockFreeRingBuffer)

	T m_data[SIZE];

	std::atomic<uint32_t> m_readIdx;
	std::atomic<uint32_t> m_writeIdx;
};
