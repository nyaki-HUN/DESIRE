#pragma once

#include "Engine/Core/Memory/Allocator.h"

// --------------------------------------------------------------------------------------------------------------------
//	A non-growing linear memory allocator
//	If the memory is exhausted, the linear allocator will use its fallback allocator to allocate memory
// --------------------------------------------------------------------------------------------------------------------

class LinearAllocator : public Allocator
{
public:
	LinearAllocator(void *memoryStart, size_t memorySize, Allocator& fallbackAllocator = Allocator::GetDefaultAllocator());

	void* Allocate(size_t size, size_t alignment = Allocator::kDefaultAlignment) final override;
	void Deallocate(void *ptr) final override;

	// Deallocate everything in O(1)
	void Reset();

private:
	char *memoryStart = nullptr;
	size_t memorySize = 0;
	size_t freeSpace = 0;
	Allocator& fallbackAllocator;
};
