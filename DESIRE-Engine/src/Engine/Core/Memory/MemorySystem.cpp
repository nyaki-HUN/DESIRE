#include "Engine/stdafx.h"
#include "Engine/Core/Memory/MemorySystem.h"
#include "Engine/Core/Memory/LinearAllocator.h"

thread_local Allocator* MemorySystem::allocatorStack[kAllocatorStackSize] = {};
thread_local size_t MemorySystem::allocatorStackIndex = 0;

// Global operator new/delete overrides
void* operator new  (size_t size)											{ return MemorySystem::Alloc(size); }
void* operator new[](size_t size)											{ return MemorySystem::Alloc(size); }
void* operator new  (size_t size, std::align_val_t alignment)				{ return MemorySystem::AlignedAlloc(size, static_cast<std::size_t>(alignment)); }
void* operator new[](size_t size, std::align_val_t alignment)				{ return MemorySystem::AlignedAlloc(size, static_cast<std::size_t>(alignment)); }

void operator delete  (void* ptr) noexcept									{ MemorySystem::Free(ptr); }
void operator delete[](void* ptr) noexcept									{ MemorySystem::Free(ptr); }
void operator delete  (void* ptr, std::align_val_t /*alignment*/) noexcept	{ MemorySystem::AlignedFree(ptr); }
void operator delete[](void* ptr, std::align_val_t /*alignment*/) noexcept	{ MemorySystem::AlignedFree(ptr); }

void* MemorySystem::Alloc(size_t size)
{
	ASSERT(size != 0);
	static_assert(kDefaultAlignment >= sizeof(AllocationHeader));

	const size_t totalSize = size + kDefaultAlignment;

	Allocator& allocator = GetActiveAllocator();
	void* allocatedMemory = allocator.Alloc(totalSize);
	ASSERT(allocatedMemory != nullptr);

	AllocationHeader& header = *reinterpret_cast<AllocationHeader*>(allocatedMemory);
	header.allocator = &allocator;
	header.allocatedSize = totalSize;

	return OffsetVoidPtr<void*>(allocatedMemory, kDefaultAlignment);
}

void* MemorySystem::Calloc(size_t num, size_t size)
{
	void* ptr = Alloc(num * size);
	if(ptr != nullptr)
	{
		memset(ptr, 0, num * size);
	}
	return ptr;
}

void* MemorySystem::Realloc(void* ptr, size_t size)
{
	if(ptr == nullptr)
	{
		return Alloc(size);
	}
	else if(size == 0)
	{
		Free(ptr);
		return nullptr;
	}

	const size_t totalSize = size + kDefaultAlignment;

	void* oldAllocatedMemory = OffsetVoidPtrBackwards<void*>(ptr, kDefaultAlignment);
	const AllocationHeader& oldHeader = *reinterpret_cast<AllocationHeader*>(oldAllocatedMemory);
	void* allocatedMemory = oldHeader.allocator->Realloc(oldAllocatedMemory, totalSize, oldHeader.allocatedSize);
	if(allocatedMemory != nullptr)
	{
		// Need to update only the size in the header because the allocator's Realloc() is responsible for copying the memory
		AllocationHeader& header = *reinterpret_cast<AllocationHeader*>(allocatedMemory);
		header.allocatedSize = totalSize;

		return OffsetVoidPtr<void*>(allocatedMemory, kDefaultAlignment);
	}

	ASSERT(false && "TODO: Use fallback allocator");
	Allocator* fallbackAllocator = nullptr;
		
	DESIRE_ALLOCATOR_SCOPE(fallbackAllocator);
	void* newPtr = Alloc(size);
	memcpy(newPtr, ptr, std::min(size, oldHeader.allocatedSize - kDefaultAlignment));
	oldHeader.allocator->Free(oldAllocatedMemory, oldHeader.allocatedSize);

	return newPtr;
}

void* MemorySystem::AlignedAlloc(size_t size, size_t alignment)
{
	return SystemAlignedAlloc(size, alignment);
}

void MemorySystem::Free(void* ptr)
{
	if(ptr == nullptr)
	{
		return;
	}

	void* allocatedMemory = OffsetVoidPtrBackwards<void*>(ptr, kDefaultAlignment);
	AllocationHeader& header = *reinterpret_cast<AllocationHeader*>(allocatedMemory);

	header.allocator->Free(allocatedMemory, header.allocatedSize);
}

void MemorySystem::AlignedFree(void* ptr)
{
	if(ptr == nullptr)
	{
		return;
	}

	return SystemAlignedFree(ptr);
}

Allocator& MemorySystem::GetActiveAllocator()
{
	return (allocatorStackIndex > 0) ? *allocatorStack[allocatorStackIndex - 1] : Allocator::GetDefaultAllocator();
}

void MemorySystem::PushAllocator(Allocator& allocator)
{
	ASSERT(allocatorStackIndex < kAllocatorStackSize);
	allocatorStack[allocatorStackIndex++] = &allocator;
}

void MemorySystem::PopAllocator()
{
	if(allocatorStackIndex > 0)
	{
		allocatorStackIndex--;
	}
	else
	{
		ASSERT(false);
	}
}

Allocator& MemorySystem::GetScratchAllocator()
{
	static constexpr size_t kScratchAllocatorSize = 10 * 1024 * 1024;
	static std::unique_ptr<char[]> data = std::make_unique<char[]>(kScratchAllocatorSize);
	static LinearAllocator s_scratchAllocator(data.get(), kScratchAllocatorSize);
	return s_scratchAllocator;
}

void MemorySystem::ResetScratchAllocator()
{
	static_cast<LinearAllocator&>(GetScratchAllocator()).Reset();
}
