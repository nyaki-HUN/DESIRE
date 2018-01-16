#include "stdafx.h"
#include "Core/memory/LinearAllocator.h"

LinearAllocator::LinearAllocator(void *memoryStart, size_t memorySize, Allocator& fallbackAllocator)
	: memoryStart((char*)memoryStart)
	, memorySize(memorySize)
	, freeSpace(memorySize)
	, fallbackAllocator(fallbackAllocator)
{

}

void* LinearAllocator::Allocate(size_t size, size_t alignment)
{
	ASSERT((alignment & (alignment - 1)) == 0 && "Alignment must be power of two");

	void *ptr = memoryStart + (memorySize - freeSpace);
	ptr = std::align(alignment, size, ptr, freeSpace);
	if(ptr == nullptr)
	{
		return fallbackAllocator.Allocate(size, alignment);
	}

	freeSpace -= size;
	return ptr;
}

void LinearAllocator::Deallocate(void *ptr)
{
	if(ptr < memoryStart || ptr >= memoryStart + memorySize)
	{
		fallbackAllocator.Deallocate(ptr);
		return;
	}

	// Do nothing
}

void LinearAllocator::Reset()
{
	freeSpace = memorySize;
}
