#include "stdafx.h"
#include "Core/memory/LinearAllocator.h"

LinearAllocator::LinearAllocator(void *memoryStart, void *memoryEnd, IAllocator& fallbackAllocator)
	: memoryStart((char*)memoryStart)
	, memoryEnd((char*)memoryEnd)
	, currMemPtr((char*)memoryStart)
	, fallbackAllocator(fallbackAllocator)
{

}

void* LinearAllocator::Allocate(size_t size, size_t alignment)
{
	char *ptr = (char*)AlignForward(currMemPtr, alignment);
	if(ptr + size <= memoryEnd)
	{
		currMemPtr = ptr + size;
		return ptr;
	}

	return fallbackAllocator.Allocate(size, alignment);
}

void LinearAllocator::Deallocate(void *ptr)
{
	if(memoryStart <= ptr && ptr < memoryEnd)
	{
		return;
	}

	fallbackAllocator.Deallocate(ptr);
}

void LinearAllocator::Reset()
{
	currMemPtr = memoryStart;
}
