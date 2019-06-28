#pragma once

#include <new>

class Allocator;

class MemorySystem
{
public:
	static constexpr size_t kDefaultAlignment = __STDCPP_DEFAULT_NEW_ALIGNMENT__;

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
		~AllocatorScope()						{ MemorySystem::PopAllocator(); }
	};

private:
	static constexpr size_t kAllocatorStackSize = 16;

	static thread_local Allocator* allocatorStack[kAllocatorStackSize];
	static thread_local size_t allocatorStackIndex;
};

#define DESIRE_ALLOCATOR_SCOPE(ALLOCATOR)	MemorySystem::AllocatorScope DESIRE_CONCAT_MACRO(allocatorScope, __COUNTER__)(ALLOCATOR)

inline size_t Align(size_t value, size_t alignment)
{
	alignment--;
	return (value + alignment) & ~alignment;
}

inline void* Align(void* ptr, size_t alignment)
{
	return reinterpret_cast<void*>(reinterpret_cast<size_t>(ptr), alignment);
}

template<typename T>
inline T* OffsetVoidPtr(const void* ptr, size_t offset)
{
	return reinterpret_cast<T*>(reinterpret_cast<size_t>(ptr) + offset);
}

template<typename T>
inline T* OffsetVoidPtrBackwards(const void* ptr, size_t offset)
{
	return reinterpret_cast<T*>(reinterpret_cast<size_t>(ptr) - offset);
}

// Global operator new/delete overrides
// Note: The behavior is undefined if more than one replacement is provided in the program or if a replacement is defined with the inline specifier.
void* operator new  (size_t size);
void* operator new[](size_t size);
void* operator new  (size_t size, std::align_val_t alignment);
void* operator new[](size_t size, std::align_val_t alignment);

void operator delete  (void* ptr) noexcept;
void operator delete[](void* ptr) noexcept;
void operator delete  (void* ptr, std::align_val_t alignment) noexcept;
void operator delete[](void* ptr, std::align_val_t alignment) noexcept;

#define malloc	MemorySystem::Alloc
#define calloc	MemorySystem::Calloc
#define realloc	MemorySystem::Realloc
#define free	MemorySystem::Free
