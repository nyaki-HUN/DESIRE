#include "Engine/stdafx.h"
#include "Engine/Core/Memory/LinearAllocator.h"
#include "Engine/Core/Memory/MemorySystem.h"

LinearAllocator::LinearAllocator(void* memoryStart, size_t memorySize, Allocator& fallbackAllocator)
	: memoryStart(static_cast<char*>(memoryStart))
	, memorySize(memorySize)
	, fallbackAllocator(fallbackAllocator)
{
}

void* LinearAllocator::Alloc(size_t size)
{
	void* ptr = memoryStart + allocatedBytes;
	size_t freeSpace = memorySize - allocatedBytes;
	if(std::align(MemorySystem::kDefaultAlignment, size, ptr, freeSpace))
	{
		const size_t offset = (memorySize - allocatedBytes) - freeSpace;
		allocatedBytes += offset + size;
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
			if(allocatedBytes + sizeDiff <= memorySize)
			{
				allocatedBytes += sizeDiff;
				return ptr;
			}
		}
		else
		{
			// Shrink the last allocation
			allocatedBytes -= oldSize - newSize;
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
		ASSERT(allocatedBytes >= size);
		allocatedBytes -= size;
	}
}

void LinearAllocator::Reset()
{
	allocatedBytes = 0;
}

bool LinearAllocator::IsAllocationOwned(const void* ptr) const
{
	return (ptr >= memoryStart && ptr < memoryStart + memorySize);
}

bool LinearAllocator::IsTheLastAllocation(const void* ptr, size_t size) const
{
	return (ptr == memoryStart + allocatedBytes - size);
}
