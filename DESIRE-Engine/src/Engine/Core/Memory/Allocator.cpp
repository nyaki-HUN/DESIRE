#include "Engine/stdafx.h"
#include "Engine/Core/Memory/Allocator.h"
#include "Engine/Core/Memory/SystemAllocator.h"

void* Allocator::Realloc(void* ptr, size_t newSize, size_t oldSize)
{
	DESIRE_ALLOCATOR_SCOPE(this);
	void* newPtr = Alloc(newSize);
	if(newPtr != nullptr)
	{
		memcpy(newPtr, ptr, std::min(oldSize, newSize));
		Free(ptr, oldSize);
	}

	return newPtr;
}

Allocator& Allocator::GetDefaultAllocator()
{
	static SystemAllocator s_defaultAllocator;
	return s_defaultAllocator;
}
