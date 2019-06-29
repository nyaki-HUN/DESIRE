#include "Engine/stdafx.h"
#include "Engine/Core/Memory/SystemAllocator.h"

void* SystemAllocator::Alloc(size_t size)
{
	return MemorySystem::SystemAlloc(size);
}

void* SystemAllocator::Realloc(void* ptr, size_t newSize, size_t oldSize)
{
	DESIRE_UNUSED(oldSize);
	return MemorySystem::SystemRealloc(ptr, newSize);
}

void SystemAllocator::Free(void* ptr, size_t size)
{
	DESIRE_UNUSED(size);
	return MemorySystem::SystemFree(ptr);
}
