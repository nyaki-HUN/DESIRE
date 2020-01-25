#include "Engine/stdafx.h"
#include "Engine/Core/Memory/MemorySystem.h"
#include "Engine/Core/Memory/LinearAllocator.h"
#include "Engine/Core/Math/math.h"

thread_local Allocator* MemorySystem::allocatorStack[kAllocatorStackSize] = {};
thread_local size_t MemorySystem::allocatorStackIndex = 0;

// Global operator new/delete overrides
// Note: The behavior is undefined if more than one replacement is provided in the program or if a replacement is defined with the inline specifier.
void* operator new  (size_t size)											{ return MemorySystem::Alloc(size, __STDCPP_DEFAULT_NEW_ALIGNMENT__); }
void* operator new[](size_t size)											{ return MemorySystem::Alloc(size, __STDCPP_DEFAULT_NEW_ALIGNMENT__); }
void* operator new  (size_t size, std::align_val_t alignment)				{ return MemorySystem::Alloc(size, static_cast<std::size_t>(alignment)); }
void* operator new[](size_t size, std::align_val_t alignment)				{ return MemorySystem::Alloc(size, static_cast<std::size_t>(alignment)); }

void operator delete  (void* ptr) noexcept									{ MemorySystem::Free(ptr); }
void operator delete[](void* ptr) noexcept									{ MemorySystem::Free(ptr); }
void operator delete  (void* ptr, std::align_val_t /*alignment*/) noexcept	{ MemorySystem::Free(ptr); }
void operator delete[](void* ptr, std::align_val_t /*alignment*/) noexcept	{ MemorySystem::Free(ptr); }

void* MemorySystem::Alloc(size_t size, size_t alignment)
{
	ASSERT(size != 0);
	ASSERT(Math::IsPowerOfTwo(alignment));
	static_assert(kDefaultAlignment >= sizeof(AllocationHeader));

	const size_t totalSize = size + std::max(kDefaultAlignment, alignment);
	Allocator& allocator = GetActiveAllocator();
	void* allocatedMemory = allocator.Alloc(totalSize);
	ASSERT(allocatedMemory != nullptr && "Out of memory");

	// Make room for the header and apply alignment
	void* ptr = Align(OffsetVoidPtr<void>(allocatedMemory, sizeof(AllocationHeader)), alignment);

	AllocationHeader* header = OffsetVoidPtrBackwards<AllocationHeader>(ptr, sizeof(AllocationHeader));
	header->allocator = &allocator;
	header->allocatedSize = Math::SafeSizeToUint32(totalSize);
	header->offsetBetweenPtrAndAllocatedMemory = Math::SafeSizeToUint32(reinterpret_cast<size_t>(ptr) - reinterpret_cast<size_t>(allocatedMemory));

	return ptr;
}

void* MemorySystem::Calloc(size_t num, size_t size)
{
	void* ptr = MemorySystem::Alloc(num * size);
	memset(ptr, 0, num * size);
	return ptr;
}

void* MemorySystem::Realloc(void* ptr, size_t size)
{
	if(size == 0)
	{
		MemorySystem::Free(ptr);
		return nullptr;
	}
	else if(ptr == nullptr)
	{
		return MemorySystem::Alloc(size);
	}

	const AllocationHeader oldHeader = *OffsetVoidPtrBackwards<AllocationHeader>(ptr, sizeof(AllocationHeader));
	void* oldAllocatedMemory = OffsetVoidPtrBackwards<void*>(ptr, oldHeader.offsetBetweenPtrAndAllocatedMemory);

	ASSERT(oldHeader.offsetBetweenPtrAndAllocatedMemory == kDefaultAlignment && "Only default alignment is supported");
	const size_t totalSize = size + kDefaultAlignment;
	if(totalSize == oldHeader.allocatedSize)
	{
		return ptr;
	}

	void* allocatedMemory = oldHeader.allocator->Realloc(oldAllocatedMemory, totalSize, oldHeader.allocatedSize);
	ASSERT(allocatedMemory != nullptr && "Out of memory");

	void* newPtr = OffsetVoidPtr<void*>(allocatedMemory, oldHeader.offsetBetweenPtrAndAllocatedMemory);

	// Only need to update the size in the header because the allocator's Realloc() is responsible for copying all the contents of the memory
	AllocationHeader* header = OffsetVoidPtrBackwards<AllocationHeader>(newPtr, sizeof(AllocationHeader));
	header->allocatedSize = Math::SafeSizeToUint32(totalSize);

	return newPtr;
}

void MemorySystem::Free(void* ptr)
{
	if(ptr == nullptr)
	{
		return;
	}

	const AllocationHeader* header = OffsetVoidPtrBackwards<AllocationHeader>(ptr, sizeof(AllocationHeader));
	void* allocatedMemory = OffsetVoidPtrBackwards<void*>(ptr, header->offsetBetweenPtrAndAllocatedMemory);

	header->allocator->Free(allocatedMemory, header->allocatedSize);
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
