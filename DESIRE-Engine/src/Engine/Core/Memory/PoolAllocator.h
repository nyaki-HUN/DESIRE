#pragma once

#include "Engine/Core/Container/FreeList.h"

template<typename T, size_t NUM_ELEMENTS>
class PoolAllocator
{
public:
	PoolAllocator()
	{
		char* pMemory = static_cast<char*>(data);
		for(size_t i = 0; i < NUM_ELEMENTS; ++i)
		{
			list.Push(pMemory);
			pMemory += kElementSize;
		}
	}

	T* Alloc()
	{
		void* pMemory = list.Pop();
		return pMemory ? new (pMemory) T() : new T();
	}

	void Free(T* pMemory)
	{
		if(data <= reinterpret_cast<char*>(pMemory) && reinterpret_cast<char*>(pMemory) <= data + (NUM_ELEMENTS - 1) * kElementSize)
		{
			pMemory->~T();
			list.Push(pMemory);
		}
		else
		{
			delete pMemory;
		}
	}

private:
	static constexpr size_t kElementSize = (sizeof(T) < sizeof(void*)) ? sizeof(void*) : sizeof(T);

	FreeList list;
	char data[NUM_ELEMENTS * kElementSize] = {};
};
