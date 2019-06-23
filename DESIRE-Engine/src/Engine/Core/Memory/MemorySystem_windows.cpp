#include "Engine/stdafx.h"

#if DESIRE_PLATFORM_WINDOWS

#include "Engine/Core/Memory/MemorySystem.h"

void* MemorySystem::SystemAlloc(size_t size)
{
	return malloc(size);
}

void* MemorySystem::SystemRealloc(void* ptr, size_t size)
{
	return realloc(ptr, size);
}

void* MemorySystem::SystemAlignedAlloc(size_t size, size_t alignment)
{
	return _aligned_malloc(size, alignment);
}

void MemorySystem::SystemFree(void* ptr)
{
	free(ptr);
}

void MemorySystem::SystemAlignedFree(void* ptr)
{
	_aligned_free(ptr);
}

#endif	// #if DESIRE_PLATFORM_WINDOWS
