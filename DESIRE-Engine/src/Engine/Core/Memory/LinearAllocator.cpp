#include "Engine/stdafx.h"
#include "Engine/Core/Memory/LinearAllocator.h"

LinearAllocator::LinearAllocator(void* memoryStart, size_t memorySize, Allocator& fallbackAllocator)
	: memoryStart(static_cast<char*>(memoryStart))
	, memorySize(memorySize)
	, freeSpace(memorySize)
	, fallbackAllocator(fallbackAllocator)
{

}

void* LinearAllocator::Alloc(size_t size)
{
	void* ptr = memoryStart + (memorySize - freeSpace);
	ptr = std::align(kDefaultAlignment, size, ptr, freeSpace);
	if(ptr != nullptr)
	{
		freeSpace -= size;
		return ptr;
	}

	return fallbackAllocator.Alloc(size);
}

void LinearAllocator::Free(void* ptr)
{
	if(ptr < memoryStart || ptr >= memoryStart + memorySize)
	{
		fallbackAllocator.Free(ptr);
		return;
	}

	// Do nothing
}

size_t LinearAllocator::MemSize(void* ptr)
{
	if(ptr < memoryStart || ptr >= memoryStart + memorySize)
	{
		return fallbackAllocator.MemSize(ptr);
	}

	// TODO
	return 0;
}

void LinearAllocator::Reset()
{
	freeSpace = memorySize;
}
