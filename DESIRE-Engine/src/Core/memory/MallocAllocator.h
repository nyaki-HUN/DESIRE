#pragma once

#include "Core/memory/IAllocator.h"

// --------------------------------------------------------------------------------------------------------------------
//	An allocator wrapper for malloc/free
// --------------------------------------------------------------------------------------------------------------------

class MallocAllocator : public IAllocator
{
public:
	MallocAllocator();

	void* Allocate(size_t size, size_t alignment = IAllocator::DEFAULT_ALIGNMENT) override;
	void Deallocate(void *ptr) override;
};
