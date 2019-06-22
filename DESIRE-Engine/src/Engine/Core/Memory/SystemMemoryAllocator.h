#pragma once

#include "Engine/Core/Memory/Allocator.h"
#include "Engine/Core/Memory/MemorySystem.h"

class SystemMemoryAllocator : public Allocator
{
public:
	void* Alloc(size_t size) final override					{ return MemorySystem::SystemAlloc(size); }
	void* Realloc(void* ptr, size_t size) final override	{ return MemorySystem::SystemRealloc(ptr, size); }
	void Free(void* ptr) final override 					{ return MemorySystem::SystemFree(ptr); }
	size_t MemSize(void* ptr) final override				{ return MemorySystem::SystemMemSize(ptr); }
};
