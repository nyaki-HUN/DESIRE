#pragma once

#include "Engine/Core/Memory/Allocator.h"

// --------------------------------------------------------------------------------------------------------------------
//	An allocator wrapper for malloc/free
// --------------------------------------------------------------------------------------------------------------------

class MallocAllocator : public Allocator
{
public:
	MallocAllocator();

	void* Allocate(size_t size, size_t alignment = Allocator::kDefaultAlignment) override;
	void Deallocate(void *ptr) override;
};
