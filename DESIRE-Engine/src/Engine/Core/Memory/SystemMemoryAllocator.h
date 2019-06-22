#pragma once

#include "Engine/Core/Memory/Allocator.h"
#include "Engine/Core/Memory/MemorySystem.h"

class SystemMemoryAllocator : public Allocator
{
public:
	void* Alloc(size_t size) override	{ return MemorySystem::Alloc(size); }
	void Free(void *ptr) override		{ return MemorySystem::Free(ptr); }
};
