#include "Engine/stdafx.h"
#include "Engine/Core/Memory/Allocator.h"

#include "Engine/Core/Memory/SystemAllocator.h"

void* Allocator::Realloc(void* pMemory, size_t newSize, size_t oldSize)
{
	void* pNewMemory = Alloc(newSize);
	memcpy(pNewMemory, pMemory, std::min(newSize, oldSize));
	Free(pMemory, oldSize);
	return pNewMemory;
}

uint64_t Allocator::GetAllocatedBytes() const
{
	return m_allocatedBytes;
}

Allocator& Allocator::GetDefaultAllocator()
{
	static SystemAllocator s_defaultAllocator;
	return s_defaultAllocator;
}
