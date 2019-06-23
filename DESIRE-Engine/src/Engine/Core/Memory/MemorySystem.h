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

	static Allocator& GetActiveAllocator();
	static void PushAllocator(Allocator& allocator);
	static void PopAllocator();

	// Returns a linear allocator which gets reset at the end of each frame
	static Allocator& GetScratchAllocator();
	// Reset all allocations in the scratch allocator (this should happen at the end of the frame)
	static void ResetScratchAllocator();

private:
	static constexpr size_t kAllocatorStackSize = 16;

	static thread_local Allocator* allocatorStack[kAllocatorStackSize];
	static thread_local size_t allocatorStackIndex;
};

class MemorySystemAllocatorScope
{
public:
	MemorySystemAllocatorScope(Allocator& allocator)	{ MemorySystem::PushAllocator(allocator); }
	~MemorySystemAllocatorScope()						{ MemorySystem::PopAllocator(); }
};

#define DESIRE_ALLOCATOR_SCOPE(ALLOCATOR)	MemorySystemAllocatorScope DESIRE_CONCAT_MACRO(allocatorScope, __COUNTER__)(ALLOCATOR)

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
