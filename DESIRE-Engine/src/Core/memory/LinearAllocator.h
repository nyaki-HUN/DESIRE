#pragma once

#include "Core/memory/IAllocator.h"

// --------------------------------------------------------------------------------------------------------------------
//	A non-growing linear memory allocator
//	If the memory is exhausted, the linear allocator will use its fallback allocator to allocate memory
// --------------------------------------------------------------------------------------------------------------------

class LinearAllocator : public IAllocator
{
public:
	LinearAllocator(void *memoryStart, size_t memorySize, IAllocator& fallbackAllocator = IAllocator::GetDefaultAllocator());

	void* Allocate(size_t size, size_t alignment = IAllocator::DEFAULT_ALIGNMENT) final override;
	void Deallocate(void *ptr) final override;

	// Deallocate everything in O(1)
	void Reset();

private:
	char *memoryStart;
	size_t memorySize;
	size_t freeSpace;
	IAllocator& fallbackAllocator;
};
