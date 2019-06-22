#include "Engine/stdafx.h"

#if DESIRE_PLATFORM_OSX

#include "Engine/Core/Memory/MemorySystem.h"

#include <malloc/malloc.h>

void* MemorySystem::SystemAlloc(size_t size)
{
	malloc_zone_t* mallocZone = malloc_default_zone();
	return mallocZone->malloc(mallocZone, size);
}

void* MemorySystem::SystemRealloc(void* ptr, size_t size)
{
	malloc_zone_t* zone = malloc_default_zone();
	return zone->realloc(zone, ptr, size);
}

void* MemorySystem::SystemAlignedAlloc(size_t size, size_t alignment)
{
	malloc_zone_t* zone = malloc_default_zone();
	return zone->memalign(zone, alignment, size);
}

void MemorySystem::SystemFree(void* ptr)
{
	malloc_zone_t* zone = malloc_zone_from_ptr(ptr);
	if(zone != nullptr)
	{
		zone->free(zone, ptr);
	}
	else
	{
		ASSERT(false && "Pointer being freed was not allocated");
	}
}

void MemorySystem::SystemAlignedFree(void* ptr)
{
	SystemFree(ptr)
}

size_t MemorySystem::SystemMemSize(void* ptr)
{
	malloc_zone_t* zone = malloc_zone_from_ptr(ptr);
	return (zone != nullptr) ? zone->size(zone, ptr) : 0;
}

#endif	// #if DESIRE_PLATFORM_OSX
