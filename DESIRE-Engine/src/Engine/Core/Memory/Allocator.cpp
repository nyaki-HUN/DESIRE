#include "Engine/stdafx.h"
#include "Engine/Core/Memory/Allocator.h"
#include "Engine/Core/Memory/SystemAllocator.h"

void* Allocator::Realloc(void* ptr, size_t newSize, size_t oldSize)
{
	void* newPtr = Alloc(newSize);
	memcpy(newPtr, ptr, std::min(newSize, oldSize));
	Free(ptr, oldSize);
	return newPtr;
}

uint64_t Allocator::GetAllocatedBytes() const
{
	return allocatedBytes;
}

Allocator& Allocator::GetDefaultAllocator()
{
	static SystemAllocator s_defaultAllocator;
	return s_defaultAllocator;
}
