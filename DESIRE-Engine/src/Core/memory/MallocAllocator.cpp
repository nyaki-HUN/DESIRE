#include "stdafx.h"
#include "Core/memory/MallocAllocator.h"

MallocAllocator::MallocAllocator()
{

}

void* MallocAllocator::Allocate(size_t size, size_t alignment)
{
#if defined(DESIRE_PLATFORM_WINDOWS)
	return _aligned_malloc(size, alignment);
#elif defined(_ISOC11_SOURCE)
	return aligned_alloc(alignment, size);
#else
	return malloc(size);
#endif
}

void MallocAllocator::Deallocate(void *ptr)
{
#if defined(DESIRE_PLATFORM_WINDOWS)
	_aligned_free(ptr);
#else
	free(ptr);
#endif
}
