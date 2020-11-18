#include "Engine/stdafx.h"
#include "Engine/Core/Memory/SystemAllocator.h"

#include "Engine/Core/Memory/MemorySystem.h"

void* SystemAllocator::Alloc(size_t size)
{
	m_allocatedBytes += size;
	return MemorySystem::SystemAlloc(size);
}

void* SystemAllocator::Realloc(void* pMemory, size_t newSize, size_t oldSize)
{
	m_allocatedBytes -= oldSize;
	m_allocatedBytes += newSize;
	return MemorySystem::SystemRealloc(pMemory, newSize);
}

void SystemAllocator::Free(void* pMemory, size_t size)
{
	m_allocatedBytes -= size;
	return MemorySystem::SystemFree(pMemory);
}
