#pragma once

class Allocator;

class MemorySystem
{
public:
	static constexpr size_t kDefaultAlignment = __STDCPP_DEFAULT_NEW_ALIGNMENT__;

	static void* Alloc(size_t size, size_t alignment = kDefaultAlignment);
	static void* Calloc(size_t num, size_t size);
	static void* Realloc(void* pMemory, size_t size);
	static void Free(void* pMemory);

	static void* SystemAlloc(size_t size);
	static void* SystemRealloc(void* pMemory, size_t size);
	static void* SystemAlignedAlloc(size_t size, size_t alignment);
	static void SystemFree(void* pMemory);
	static void SystemAlignedFree(void* pMemory);

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
		AllocatorScope(Allocator* pAllocator)	{ MemorySystem::PushAllocator(*pAllocator); }
		~AllocatorScope()						{ MemorySystem::PopAllocator(); }
	};

private:
	struct AllocationHeader
	{
		Allocator* pAllocator;
		uint32_t allocatedSize;
		uint32_t offsetBetweenPtrAndAllocatedMemory;
	};
};

#define DESIRE_ALLOCATOR_SCOPE(ALLOCATOR)	MemorySystem::AllocatorScope DESIRE_CONCAT_MACRO(allocatorScope, __COUNTER__)(ALLOCATOR)
