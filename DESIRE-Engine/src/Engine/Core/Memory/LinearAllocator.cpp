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
	void* ptr = memoryStart + memorySize - freeSpace;
	ptr = std::align(MemorySystem::kDefaultAlignment, size, ptr, freeSpace);
	if(ptr != nullptr)
	{
		freeSpace -= size;
		return ptr;
	}

	return fallbackAllocator.Alloc(size);
}

void* LinearAllocator::Realloc(void* ptr, size_t newSize, size_t oldSize)
{
	if(!IsAllocationOwned(ptr))
	{
		return fallbackAllocator.Realloc(ptr, newSize, oldSize);
	}

	if(IsTheLastAllocation(ptr, oldSize))
	{
		if(newSize > oldSize)
		{
			// Try to grow the last allocation
			const size_t sizeDiff = newSize - oldSize;
			if(sizeDiff <= freeSpace)
			{
				freeSpace -= sizeDiff;
				return ptr;
			}
		}
		else
		{
			// Shrink the last allocation
			const size_t sizeDiff = oldSize - newSize;
			freeSpace += sizeDiff;
			return ptr;
		}
	}

	void* newPtr = Alloc(newSize);
	memcpy(newPtr, ptr, std::min(newSize, oldSize));
	return newPtr;
}

void LinearAllocator::Free(void* ptr, size_t size)
{
	if(!IsAllocationOwned(ptr))
	{
		fallbackAllocator.Free(ptr, size);
		return;
	}

	if(IsTheLastAllocation(ptr, size))
	{
		const size_t totalSize = Align(size, MemorySystem::kDefaultAlignment);
		freeSpace += totalSize;
		ASSERT(freeSpace <= memorySize);
	}
}

void LinearAllocator::Reset()
{
	freeSpace = memorySize;
}

bool LinearAllocator::IsAllocationOwned(const void* ptr) const
{
	return (ptr >= memoryStart && ptr < memoryStart + memorySize);
}

bool LinearAllocator::IsTheLastAllocation(const void* ptr, size_t size) const
{
	return (ptr == memoryStart + memorySize - freeSpace - size);
}
