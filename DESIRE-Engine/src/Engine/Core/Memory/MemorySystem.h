#pragma once

class Allocator;

class MemorySystem
{
public:
	static constexpr size_t kDefaultAlignment = __STDCPP_DEFAULT_NEW_ALIGNMENT__;

	static void* Alloc(size_t size, size_t alignment = kDefaultAlignment);
	static void* Calloc(size_t num, size_t size);
	static void* Realloc(void* ptr, size_t size);
	static void Free(void* ptr);

	static void* SystemAlloc(size_t size);
	static void* SystemRealloc(void* ptr, size_t size);
	static void* SystemAlignedAlloc(size_t size, size_t alignment);
	static void SystemFree(void* ptr);
	static void SystemAlignedFree(void* ptr);

	static Allocator& GetActiveAllocator();
	static void PushAllocator(Allocator& allocator);
	static void PopAllocator();

	// Returns a linear allocator which gets reset at the end of each frame
	static Allocator& GetScratchAllocator();
	// Reset all allocations in the scratch allocator (this should happen at the end of the frame)
	static void ResetScratchAllocator();

	struct AllocatorScope
	{
		AllocatorScope(Allocator& allocator)	{ MemorySystem::PushAllocator(allocator); }
		AllocatorScope(Allocator* allocator)	{ MemorySystem::PushAllocator(*allocator); }
		~AllocatorScope()						{ MemorySystem::PopAllocator(); }
	};

private:
	struct AllocationHeader
	{
		Allocator* allocator;
		uint32_t allocatedSize;
		uint32_t offsetBetweenPtrAndAllocatedMemory;
	};

	static constexpr size_t kAllocatorStackSize = 16;

	static thread_local Allocator* allocatorStack[kAllocatorStackSize];
	static thread_local size_t allocatorStackIndex;
};

#define DESIRE_ALLOCATOR_SCOPE(ALLOCATOR)	MemorySystem::AllocatorScope DESIRE_CONCAT_MACRO(allocatorScope, __COUNTER__)(ALLOCATOR)
