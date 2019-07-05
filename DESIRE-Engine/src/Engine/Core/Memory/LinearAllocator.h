#pragma once

#include "Engine/Core/Memory/Allocator.h"

// --------------------------------------------------------------------------------------------------------------------
//	A non-growing linear memory allocator
//	If the memory is exhausted, the linear allocator will use its fallback allocator to allocate memory
// --------------------------------------------------------------------------------------------------------------------

class LinearAllocator : public Allocator
{
public:
	LinearAllocator(void* memoryStart, size_t memorySize, Allocator& fallbackAllocator = Allocator::GetDefaultAllocator());

	void* Alloc(size_t size) final override;
	void* Realloc(void* ptr, size_t newSize, size_t oldSize) final override;
	void Free(void* ptr, size_t size) final override;

	// Free everything in O(1)
	void Reset();

protected:
	bool IsAllocationOwned(const void* ptr) const;

	char* memoryStart = nullptr;
	size_t memorySize = 0;
	size_t freeSpace = 0;
	void* lastAllocation = nullptr;
	Allocator& fallbackAllocator;
};
