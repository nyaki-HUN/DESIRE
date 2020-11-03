#include "Engine/stdafx.h"
#include "Engine/Core/Memory/MemorySystem.h"

#include "Engine/Core/Math/math.h"
#include "Engine/Core/Memory/LinearAllocator.h"

static constexpr size_t kAllocatorStackSize = 16;
static thread_local Allocator* s_allocatorStack[kAllocatorStackSize] = {};
static thread_local size_t s_allocatorStackIndex = 0;

// Helper functions
static void* OffsetVoidPtr(const void* pMemory, size_t offset)				{ return reinterpret_cast<void*>(reinterpret_cast<size_t>(pMemory) + offset); }
static void* OffsetVoidPtrBackwards(const void* pMemory, size_t offset)		{ return reinterpret_cast<void*>(reinterpret_cast<size_t>(pMemory) - offset); }
template<typename T> static T* OffsetVoidPtr(const void* pMemory)			{ return reinterpret_cast<T*>(OffsetVoidPtr(pMemory, sizeof(T))); }
template<typename T> static T* OffsetVoidPtrBackwards(const void* pMemory)	{ return reinterpret_cast<T*>(OffsetVoidPtrBackwards(pMemory, sizeof(T))); }

static size_t Align(size_t value, size_t alignment)							{ alignment--; return (value + alignment) & ~alignment; }
static void* Align(void* pMemory, size_t alignment)							{ return reinterpret_cast<void*>(Align(reinterpret_cast<size_t>(pMemory), alignment)); }

void* MemorySystem::Alloc(size_t size, size_t alignment)
{
	ASSERT(Math::IsPowerOfTwo(alignment));
	static_assert(kDefaultAlignment >= sizeof(AllocationHeader));

	if(size != 0)
	{
		const size_t totalSize = size + std::max(kDefaultAlignment, alignment);
		Allocator& allocator = GetActiveAllocator();
		void* pAllocatedMemory = allocator.Alloc(totalSize);
		if(pAllocatedMemory != nullptr)
		{
			// Make room for the header and apply alignment
			void* pMemory = Align(OffsetVoidPtr(pAllocatedMemory, sizeof(AllocationHeader)), alignment);

			AllocationHeader* header = OffsetVoidPtrBackwards<AllocationHeader>(pMemory);
			header->pAllocator = &allocator;
			header->allocatedSize = Math::SafeSizeToUint32(totalSize);
			header->offsetBetweenPtrAndAllocatedMemory = Math::SafeSizeToUint32(reinterpret_cast<size_t>(pMemory) - reinterpret_cast<size_t>(pAllocatedMemory));

			return pMemory;
		}

		ASSERT(false && "Out of memory");
	}

	return nullptr;
}

void* MemorySystem::Calloc(size_t num, size_t size)
{
	void* pMemory = MemorySystem::Alloc(num * size);
	if(pMemory != nullptr)
	{
		memset(pMemory, 0, num * size);
	}

	return pMemory;
}

void* MemorySystem::Realloc(void* pMemory, size_t size)
{
	if(size == 0)
	{
		MemorySystem::Free(pMemory);
		return nullptr;
	}
	else if(pMemory == nullptr)
	{
		return MemorySystem::Alloc(size);
	}

	const AllocationHeader oldHeader = *OffsetVoidPtrBackwards<AllocationHeader>(pMemory);
	ASSERT(oldHeader.offsetBetweenPtrAndAllocatedMemory != 0xFDFDFDFD && "Windows Debug Heap's NoMansLand buffer detected. The memory was not allocated by the MemorySystem");
	void* oldAllocatedMemory = OffsetVoidPtrBackwards(pMemory, oldHeader.offsetBetweenPtrAndAllocatedMemory);

	ASSERT(oldHeader.offsetBetweenPtrAndAllocatedMemory == kDefaultAlignment && "Only default alignment is supported");
	const size_t totalSize = size + kDefaultAlignment;
	if(totalSize == oldHeader.allocatedSize)
	{
		return pMemory;
	}

	void* allocatedMemory = oldHeader.pAllocator->Realloc(oldAllocatedMemory, totalSize, oldHeader.allocatedSize);
	if(allocatedMemory != nullptr)
	{
		void* newPtr = OffsetVoidPtr(allocatedMemory, oldHeader.offsetBetweenPtrAndAllocatedMemory);

		// Only need to update the size in the header because the allocator's Realloc() is responsible for copying all the contents of the memory
		AllocationHeader* pHeader = OffsetVoidPtrBackwards<AllocationHeader>(newPtr);
		pHeader->allocatedSize = Math::SafeSizeToUint32(totalSize);

		return newPtr;
	}

	ASSERT(false && "Out of memory");
	return nullptr;
}

void MemorySystem::Free(void* pMemory)
{
	if(pMemory == nullptr)
	{
		return;
	}

	const AllocationHeader* header = OffsetVoidPtrBackwards<AllocationHeader>(pMemory);
	ASSERT(header->offsetBetweenPtrAndAllocatedMemory != 0xFDFDFDFD && "Windows Debug Heap's NoMansLand buffer detected. The memory was not allocated by the MemorySystem");
	void* allocatedMemory = OffsetVoidPtrBackwards(pMemory, header->offsetBetweenPtrAndAllocatedMemory);

	header->pAllocator->Free(allocatedMemory, header->allocatedSize);
}

Allocator& MemorySystem::GetActiveAllocator()
{
	return (s_allocatorStackIndex > 0) ? *s_allocatorStack[s_allocatorStackIndex - 1] : Allocator::GetDefaultAllocator();
}

void MemorySystem::PushAllocator(Allocator& allocator)
{
	ASSERT(s_allocatorStackIndex < kAllocatorStackSize);
	s_allocatorStack[s_allocatorStackIndex++] = &allocator;
}

void MemorySystem::PopAllocator()
{
	if(s_allocatorStackIndex > 0)
	{
		s_allocatorStackIndex--;
	}
	else
	{
		ASSERT(false);
	}
}

Allocator& MemorySystem::GetScratchAllocator()
{
	constexpr size_t kScratchAllocatorSize = 10 * 1024 * 1024;
	static std::unique_ptr<uint8_t[]> data = std::make_unique<uint8_t[]>(kScratchAllocatorSize);
	static LinearAllocator s_scratchAllocator(data.get(), kScratchAllocatorSize);
	return s_scratchAllocator;
}

void MemorySystem::ResetScratchAllocator()
{
	static_cast<LinearAllocator&>(GetScratchAllocator()).Reset();
}
