#pragma once

#include "Core/memory/LinearAllocator.h"

// --------------------------------------------------------------------------------------------------------------------
//	StackAllocator is a scoped linear allocator that reserves STACK_SIZE amount of local stack memory
// --------------------------------------------------------------------------------------------------------------------

template<size_t STACK_SIZE>
class StackAllocator : public LinearAllocator
{
public:
	StackAllocator()
		: LinearAllocator(data, data + STACK_SIZE)
	{

	}

private:
	// Prevent heap allocation
	void* operator new(size_t) = delete;
	void* operator new[](size_t) = delete;

	char data[STACK_SIZE];
};
