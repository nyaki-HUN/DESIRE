#pragma once

#include "Engine/Core/Memory/Allocator.h"

// --------------------------------------------------------------------------------------------------------------------
//	A non-growing linear memory allocator
//	If the memory is exhausted, the linear allocator will use its fallback allocator to allocate memory
// --------------------------------------------------------------------------------------------------------------------

class LinearAllocator : public Allocator
{
public:
	LinearAllocator(void* pMemoryStart, size_t memorySize, Allocator& fallbackAllocator = Allocator::GetDefaultAllocator());

	void* Alloc(size_t size) final override;
	void* Realloc(void* pMemory, size_t newSize, size_t oldSize) final override;
	void Free(void* pMemory, size_t size) final override;

	// Free everything in O(1)
	void Reset();

	bool IsMemoryFromThis(const void* pMemory) const;

protected:
	bool IsTheLastAllocation(const void* pMemory, size_t size) const;

	uint8_t* m_pMemoryStart = nullptr;
	size_t m_memorySize = 0;
	Allocator& m_fallbackAllocator;
};
