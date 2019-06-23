#include "Engine/stdafx.h"
#include "Engine/Core/Memory/Allocator.h"

Allocator Allocator::s_defaultAllocator;

Allocator::Allocator()
{
}

Allocator::~Allocator()
{
}

void* Allocator::Alloc(size_t size)
{
	return MemorySystem::SystemAlloc(size);
}

void* Allocator::Realloc(void* ptr, size_t size)
{
	return MemorySystem::SystemRealloc(ptr, size);
}

void Allocator::Free(void* ptr)
{
	return MemorySystem::SystemFree(ptr);
}

Allocator& Allocator::GetDefaultAllocator()
{
	return s_defaultAllocator;
}
