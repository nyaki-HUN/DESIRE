#pragma once

#include "Engine/Core/Memory/Allocator.h"

class SystemAllocator : public Allocator
{
public:
	void* Alloc(size_t size) final override;
	void* Realloc(void* pMemory, size_t newSize, size_t oldSize) final override;
	void Free(void* pMemory, size_t size) final override;
};
