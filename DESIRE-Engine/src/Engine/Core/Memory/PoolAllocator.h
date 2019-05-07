#pragma once

#include "Engine/Core/Container/FreeList.h"

template<typename T, size_t NUM_ELEMENTS>
class PoolAllocator
{
public:
	PoolAllocator()
	{
		char *ptr = static_cast<char*>(data);
		for(size_t i = 0; i < NUM_ELEMENTS; ++i)
		{
			list.Push(ptr);
			ptr += kElementSize;
		}
	}

	T* PoolAllocator::Allocate()
	{
		void *memory = list.Pop();
		return (memory != nullptr) ? new (memory) T() : new T();
	}

	void PoolAllocator::Deallocate(T *ptr)
	{
		if(data <= reinterpret_cast<char*>(ptr) && reinterpret_cast<char*>(ptr) <= data + (NUM_ELEMENTS - 1) * kElementSize)
		{
			ptr->~T();
			list.Push(ptr);
		}
		else
		{
			delete ptr;
		}
	}

private:
	static constexpr size_t kElementSize = (sizeof(T) < sizeof(void*)) ? sizeof(void*) : sizeof(T);

	FreeList list;
	char data[NUM_ELEMENTS * kElementSize] = {};
};
