#include "Engine/stdafx.h"
#include "Engine/Core/Memory/MemorySystem.h"
#include "Engine/Core/Memory/LinearAllocator.h"

void* MemorySystem::Alloc(size_t size)
{
	Allocator& allocator = Allocator::GetDefaultAllocator();
	return allocator.Alloc(size);
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
	Allocator& allocator = Allocator::GetDefaultAllocator();
	return allocator.Realloc(ptr, size);
}

void* MemorySystem::AlignedAlloc(size_t size, size_t alignment)
{
	return SystemAlignedAlloc(size, alignment);
}

void MemorySystem::Free(void* ptr)
{
	Allocator& allocator = Allocator::GetDefaultAllocator();
	return allocator.Free(ptr);
}

void MemorySystem::AlignedFree(void* ptr)
{
	return SystemAlignedFree(ptr);
}
