#pragma once

#include "Engine/Core/memory/Allocator.h"

// --------------------------------------------------------------------------------------------------------------------
//	An allocator wrapper for malloc/free
// --------------------------------------------------------------------------------------------------------------------

class MallocAllocator : public Allocator
{
public:
	MallocAllocator();

	void* Allocate(size_t size, size_t alignment = Allocator::DEFAULT_ALIGNMENT) override;
	void Deallocate(void *ptr) override;
};
