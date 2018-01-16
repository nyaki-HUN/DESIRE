#pragma once

#include "Engine/Core/memory/FreeList.h"

#include <algorithm>	// for std::max

template<typename T, size_t NUM_ELEMENTS>
class PoolAllocator
{
public:
	PoolAllocator()
		: list(data, sizeof(data), std::max(sizeof(T), sizeof(void*)))
	{

	}

	inline T* PoolAllocator::Allocate()
	{
		T *ptr = new (list.ObtainElement()) T();
		return ptr;
	}

	inline void PoolAllocator::Deallocate(T *ptr)
	{
		ptr->~T();
		list.ReturnElement(ptr);
	}

private:
	FreeList list;
	char data[NUM_ELEMENTS * std::max(sizeof(T), sizeof(void*))];
};
