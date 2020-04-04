#include "Engine/stdafx.h"
#include "Engine/Core/Memory/SystemAllocator.h"

#include "Engine/Core/Memory/MemorySystem.h"

void* SystemAllocator::Alloc(size_t size)
{
	allocatedBytes += size;
	return MemorySystem::SystemAlloc(size);
}

void* SystemAllocator::Realloc(void* ptr, size_t newSize, size_t oldSize)
{
	allocatedBytes -= oldSize;
	allocatedBytes += newSize;
	return MemorySystem::SystemRealloc(ptr, newSize);
}

void SystemAllocator::Free(void* ptr, size_t size)
{
	allocatedBytes -= size;
	return MemorySystem::SystemFree(ptr);
}
