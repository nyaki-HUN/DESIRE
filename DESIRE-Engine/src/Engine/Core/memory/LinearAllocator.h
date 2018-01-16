#pragma once

#include "Engine/Core/memory/Allocator.h"

// --------------------------------------------------------------------------------------------------------------------
//	A non-growing linear memory allocator
//	If the memory is exhausted, the linear allocator will use its fallback allocator to allocate memory
// --------------------------------------------------------------------------------------------------------------------

class LinearAllocator : public Allocator
{
public:
	LinearAllocator(void *memoryStart, size_t memorySize, Allocator& fallbackAllocator = Allocator::GetDefaultAllocator());

	void* Allocate(size_t size, size_t alignment = Allocator::DEFAULT_ALIGNMENT) final override;
	void Deallocate(void *ptr) final override;

	// Deallocate everything in O(1)
	void Reset();

private:
	char *memoryStart;
	size_t memorySize;
	size_t freeSpace;
	Allocator& fallbackAllocator;
};
