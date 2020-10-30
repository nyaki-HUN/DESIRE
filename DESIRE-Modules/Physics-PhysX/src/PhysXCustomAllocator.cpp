#include "stdafx_PhysX.h"
#include "PhysXCustomAllocator.h"

#include "Engine/Core/Memory/MemorySystem.h"

void* PhysXCustomAllocator::allocate(size_t size, const char* pTypeName, const char* pFilename, int line)
{
	DESIRE_UNUSED(pTypeName);
	DESIRE_UNUSED(pFilename);
	DESIRE_UNUSED(line);

	// 16-byte alignment is required by the SDK
	return MemorySystem::Alloc(size, 16);
}

void PhysXCustomAllocator::deallocate(void* pMemory)
{
	MemorySystem::Free(pMemory);
}
