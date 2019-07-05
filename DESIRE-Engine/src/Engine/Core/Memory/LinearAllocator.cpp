#include "Engine/stdafx.h"
#include "Engine/Core/Memory/LinearAllocator.h"
#include "Engine/Core/Memory/MemorySystem.h"

LinearAllocator::LinearAllocator(void* memoryStart, size_t memorySize, Allocator& fallbackAllocator)
	: memoryStart(static_cast<char*>(memoryStart))
	, memorySize(memorySize)
	, freeSpace(memorySize)
	, fallbackAllocator(fallbackAllocator)
{
}

void* LinearAllocator::Alloc(size_t size)
{
	const size_t totalSize = Align(size, MemorySystem::kDefaultAlignment);
	if(totalSize <= freeSpace)
	{
		void* ptr = memoryStart + (memorySize - freeSpace);

		freeSpace -= totalSize;
		lastAllocation = ptr;
		return ptr;
	}

	return fallbackAllocator.Alloc(size);
}

void* LinearAllocator::Realloc(void* ptr, size_t newSize, size_t oldSize)
{
	ASSERT(IsAllocationOwned(ptr));

	if(lastAllocation == ptr)
	{
		// Try to grow the last allocation
		const size_t sizeDiff = Align(newSize, MemorySystem::kDefaultAlignment) - Align(oldSize, MemorySystem::kDefaultAlignment);
		if(sizeDiff <= freeSpace)
		{
			freeSpace -= sizeDiff;
			return ptr;
		}
	}

	void* newPtr = Alloc(newSize);
	if(newPtr != nullptr)
	{
		memcpy(newPtr, ptr, std::min(newSize, oldSize));
	}
	return newPtr;
}

void LinearAllocator::Free(void* ptr, size_t size)
{
	ASSERT(IsAllocationOwned(ptr));

	if(lastAllocation == ptr)
	{
		const size_t totalSize = Align(size, MemorySystem::kDefaultAlignment);
		freeSpace += totalSize;
		ASSERT(freeSpace <= memorySize);
		lastAllocation = (freeSpace < memorySize) ? OffsetVoidPtrBackwards<void>(lastAllocation, totalSize) : nullptr;
	}
}

void LinearAllocator::Reset()
{
	lastAllocation = nullptr;
	freeSpace = memorySize;
}

bool LinearAllocator::IsAllocationOwned(const void* ptr) const
{
	return (ptr >= memoryStart && ptr < memoryStart + memorySize);
}
