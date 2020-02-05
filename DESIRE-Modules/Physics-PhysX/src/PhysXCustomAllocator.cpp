#include "stdafx_PhysX.h"
#include "PhysXCustomAllocator.h"

#include "Engine/Core/Memory/MemorySystem.h"

void* PhysXCustomAllocator::allocate(size_t size, const char* typeName, const char* filename, int line)
{
	DESIRE_UNUSED(typeName);
	DESIRE_UNUSED(filename);
	DESIRE_UNUSED(line);

	// 16-byte alignment is required by the SDK
	return MemorySystem::Alloc(size, 16);
}

void PhysXCustomAllocator::deallocate(void* ptr)
{
	MemorySystem::Free(ptr);
}
