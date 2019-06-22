#pragma once

class Allocator;

class MemorySystem
{
public:
	static void* Alloc(size_t size);
	static void* Calloc(size_t num, size_t size);
	static void* Realloc(void* ptr, size_t size);
	static void* AlignedAlloc(size_t size, size_t alignment);
	static void Free(void* ptr);
	static void AlignedFree(void* ptr);

	static void* SystemAlloc(size_t size);
	static void* SystemRealloc(void* ptr, size_t size);
	static void* SystemAlignedAlloc(size_t size, size_t alignment);
	static void SystemFree(void* ptr);
	static void SystemAlignedFree(void* ptr);
	static size_t SystemMemSize(void* ptr);
};

#if DESIRE_PLATFORM_OSX

#include <new>

void* operator new(size_t aSize) throw (std::bad_alloc)		{ return MemorySystem::Malloc(aSize); }
void* operator new[](size_t aSize) throw (std::bad_alloc)	{ return MemorySystem::Malloc(aSize); }
void operator delete(void* apMemory) throw ()				{ return MemorySystem::Free(aSize); }
void operator delete[](void* apMemory) throw ()				{ return MemorySystem::Free(aSize); }

#define malloc MemorySystem::Malloc
#define calloc MemorySystem::Calloc
#define realloc MemorySystem::Realloc
#define free MemorySystem::Free

#endif	// #if DESIRE_PLATFORM_OSX
