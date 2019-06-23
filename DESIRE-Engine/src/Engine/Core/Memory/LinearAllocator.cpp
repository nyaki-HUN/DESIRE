#include "Engine/stdafx.h"
#include "Engine/Core/Memory/LinearAllocator.h"

uint32_t SizeToUint32Safe(size_t value)
{
	ASSERT(value <= UINT32_MAX);
	return static_cast<uint32_t>(value);
}

size_t Align(size_t value, size_t alignment)
{
	return value + (alignment - 1) & ~alignment;
}

void* Align(void* ptr, size_t alignment)
{
	return reinterpret_cast<void*>(reinterpret_cast<size_t>(ptr), alignment);
}

template<typename T>
T* OffsetVoidPtr(const void* ptr, size_t offset)
{
	return reinterpret_cast<T*>(reinterpret_cast<size_t>(ptr) + offset);
}

template<typename T>
T* OffsetVoidPtrBackwards(const void* ptr, size_t offset)
{
	return reinterpret_cast<T*>(reinterpret_cast<size_t>(ptr) - offset);
}

LinearAllocator::LinearAllocator(void* memoryStart, size_t memorySize, Allocator& fallbackAllocator)
	: memoryStart(static_cast<char*>(memoryStart))
	, memorySize(memorySize)
	, freeSpace(memorySize)
	, fallbackAllocator(fallbackAllocator)
{
	static_assert(kDefaultAlignment == sizeof(AllocationHeader), "AllocationHeader's size has to be equal to kDefaultAlignment");
}

void* LinearAllocator::Alloc(size_t size)
{
	const uint32_t totalSize = SizeToUint32Safe(Align(size, kDefaultAlignment) + sizeof(AllocationHeader));
	if(totalSize <= freeSpace)
	{
		void* ptr = memoryStart + (memorySize - freeSpace) + sizeof(AllocationHeader);
		AllocationHeader& header = *OffsetVoidPtrBackwards<AllocationHeader>(ptr, sizeof(AllocationHeader));
		header.totalSize = totalSize;
		header.offsetToPrev = SizeToUint32Safe(reinterpret_cast<size_t>(ptr) - reinterpret_cast<size_t>(lastAllocation));

		freeSpace -= totalSize;
		lastAllocation = ptr;
		return ptr;
	}

	return fallbackAllocator.Alloc(size);
}

void* LinearAllocator::Realloc(void* ptr, size_t size)
{
	if(ptr == nullptr)
	{
		return Alloc(size);
	}

	if(IsAllocationOwned(ptr))
	{
		const uint32_t totalSize = SizeToUint32Safe(Align(size, kDefaultAlignment) + sizeof(AllocationHeader));
		AllocationHeader& header = *OffsetVoidPtrBackwards<AllocationHeader>(ptr, sizeof(AllocationHeader));

		if(totalSize <= header.totalSize)
		{
			// Shrink an existing allocation only if it was the last
			if(lastAllocation == ptr)
			{
				const uint32_t sizeDiff = header.totalSize - totalSize;
				header.totalSize -= sizeDiff;
				freeSpace += sizeDiff;
			}

			return ptr;
		}

		if(lastAllocation == ptr)
		{
			// Try to grow the last allocation
			const uint32_t sizeDiff = totalSize - header.totalSize;
			if(sizeDiff <= freeSpace)
			{
				header.totalSize += sizeDiff;
				freeSpace -= sizeDiff;
				return ptr;
			}
		}

		void* newPtr = Alloc(size);
		memcpy(newPtr, ptr, header.totalSize - sizeof(AllocationHeader));
		Free(ptr);
		return newPtr;
	}

	return fallbackAllocator.Realloc(ptr, size);
}

void LinearAllocator::Free(void* ptr)
{
	if(IsAllocationOwned(ptr))
	{
		if(lastAllocation == ptr)
		{
			const AllocationHeader& header = *OffsetVoidPtrBackwards<AllocationHeader>(ptr, sizeof(AllocationHeader));
			lastAllocation = OffsetVoidPtrBackwards<void>(lastAllocation, header.offsetToPrev);
			freeSpace += header.totalSize;
		}

		return;
	}

	fallbackAllocator.Free(ptr);
}

void LinearAllocator::Reset()
{
	lastAllocation = nullptr;
	freeSpace = memorySize;
}

bool LinearAllocator::IsAllocationOwned(const void* ptr) const
{
	return (ptr >= memoryStart && ptr < memoryStart + memorySize);
}

size_t LinearAllocator::GetAllocationTotalSize(const void* ptr) const
{
	ASSERT(IsAllocationOwned(ptr));

	const AllocationHeader& header = *OffsetVoidPtrBackwards<AllocationHeader>(ptr, sizeof(AllocationHeader));
	return header.totalSize;
}
