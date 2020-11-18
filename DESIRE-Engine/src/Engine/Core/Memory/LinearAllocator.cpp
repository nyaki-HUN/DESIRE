#include "Engine/stdafx.h"
#include "Engine/Core/Memory/LinearAllocator.h"

#include "Engine/Core/Memory/MemorySystem.h"

LinearAllocator::LinearAllocator(void* pMemoryStart, size_t memorySize, Allocator& fallbackAllocator)
	: m_pMemoryStart(static_cast<uint8_t*>(pMemoryStart))
	, m_memorySize(memorySize)
	, m_fallbackAllocator(fallbackAllocator)
{
}

void* LinearAllocator::Alloc(size_t size)
{
	void* pMemory = m_pMemoryStart + m_allocatedBytes;
	size_t bufferSize = m_memorySize - m_allocatedBytes;
	if(std::align(MemorySystem::kDefaultAlignment, size, pMemory, bufferSize))
	{
		const size_t offset = (m_memorySize - m_allocatedBytes) - bufferSize;
		m_allocatedBytes += offset + size;
		return pMemory;
	}

	return m_fallbackAllocator.Alloc(size);
}

void* LinearAllocator::Realloc(void* pMemory, size_t newSize, size_t oldSize)
{
	if(!IsAllocationOwned(pMemory))
	{
		return m_fallbackAllocator.Realloc(pMemory, newSize, oldSize);
	}

	if(IsTheLastAllocation(pMemory, oldSize))
	{
		if(newSize > oldSize)
		{
			// Try to grow the last allocation
			const size_t sizeDiff = newSize - oldSize;
			if(m_allocatedBytes + sizeDiff <= m_memorySize)
			{
				m_allocatedBytes += sizeDiff;
				return pMemory;
			}
		}
		else
		{
			// Shrink the last allocation
			m_allocatedBytes -= oldSize - newSize;
			return pMemory;
		}
	}

	void* newPtr = Alloc(newSize);
	memcpy(newPtr, pMemory, std::min(newSize, oldSize));
	return newPtr;
}

void LinearAllocator::Free(void* pMemory, size_t size)
{
	if(!IsAllocationOwned(pMemory))
	{
		m_fallbackAllocator.Free(pMemory, size);
		return;
	}

	if(IsTheLastAllocation(pMemory, size))
	{
		ASSERT(m_allocatedBytes >= size);
		m_allocatedBytes -= size;
	}
}

void LinearAllocator::Reset()
{
	m_allocatedBytes = 0;
}

bool LinearAllocator::IsAllocationOwned(const void* pMemory) const
{
	return (pMemory >= m_pMemoryStart && pMemory < m_pMemoryStart + m_memorySize);
}

bool LinearAllocator::IsTheLastAllocation(const void* pMemory, size_t size) const
{
	return (pMemory == m_pMemoryStart + m_allocatedBytes - size);
}
