#pragma once

#include "Engine/Core/assert.h"

#include <atomic>
#include <utility>		// for std::move()

// --------------------------------------------------------------------------------------------------------------------
//	LockFreeRingBuffer is a one producer and one consumer queue without locks.
//	Note that the number of element slots in the buffer is actually (SIZE - 1).
// --------------------------------------------------------------------------------------------------------------------

template<typename T, uint32_t SIZE>
class LockFreeRingBuffer
{
public:
	LockFreeRingBuffer()
		: readIdx(0)
		, writeIdx(0)
	{
		static_assert(SIZE >= 2, "Size must be at least 2");
	}

	// No synchronization needed in destructor as only one thread can do this
	~LockFreeRingBuffer() {}

	// Add new element at the end of the buffer, after its current last element
	// The content of value is copied to the new element
	bool push(const T& value)
	{
		const uint32_t currWrite = writeIdx.load(std::memory_order_relaxed);
		const uint32_t nextIdx = (currWrite + 1) % SIZE;
		if(nextIdx == readIdx.load(std::memory_order_acquire))
		{
			// The buffer is full
			return false;
		}

		data[currWrite] = value;
		writeIdx.store(nextIdx, std::memory_order_release);
		return true;
	}

	// Add new element at the end of the buffer, after its current last element
	// The content of value is moved to the new element
	bool push(T&& value)
	{
		const uint32_t currWrite = writeIdx.load(std::memory_order_relaxed);
		const uint32_t nextIdx = (currWrite + 1) % SIZE;
		if(nextIdx == readIdx.load(std::memory_order_acquire))
		{
			// The buffer is full
			return false;
		}

		data[currWrite] = std::move(value);
		writeIdx.store(nextIdx, std::memory_order_release);
		return true;
	}

	// Remove the next element in the buffer by moving it to the given variable
	bool pop(T& value)
	{
		const uint32_t currRead = readIdx.load(std::memory_order_relaxed);
		if(currRead == writeIdx.load(std::memory_order_acquire))
		{
			// The buffer is empty
			return false;
		}

		const uint32_t nextIdx = (currRead + 1) % SIZE;
		value = std::move(data[currRead]);
		readIdx.store(nextIdx, std::memory_order_release);
		return true;
	}

	// Returns true if there is no element in the buffer
	bool empty() const
	{
		return readIdx.load(std::memory_order_consume) == writeIdx.load(std::memory_order_consume);
	}

private:
	// Prevent copy and move
	LockFreeRingBuffer(const LockFreeRingBuffer& other) = delete;
	LockFreeRingBuffer(LockFreeRingBuffer&& other) = delete;
	LockFreeRingBuffer& operator=(const LockFreeRingBuffer& other) = delete;
	LockFreeRingBuffer& operator=(LockFreeRingBuffer&& other) = delete;

	T data[SIZE];

	std::atomic<uint32_t> readIdx;
	std::atomic<uint32_t> writeIdx;
};
