#include "Engine/stdafx.h"
#include "Engine/Core/Memory/MemorySystem.h"

#if DESIRE_PLATFORM_WINDOWS

void* MemorySystem::SystemAlloc(size_t size)
{
	return malloc(size);
}

void* MemorySystem::SystemRealloc(void* pMemory, size_t size)
{
	return realloc(pMemory, size);
}

void* MemorySystem::SystemAlignedAlloc(size_t size, size_t alignment)
{
	return _aligned_malloc(size, alignment);
}

void MemorySystem::SystemFree(void* pMemory)
{
	free(pMemory);
}

void MemorySystem::SystemAlignedFree(void* pMemory)
{
	_aligned_free(pMemory);
}

#endif	// #if DESIRE_PLATFORM_WINDOWS
