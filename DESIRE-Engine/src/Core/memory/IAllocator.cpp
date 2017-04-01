#include "stdafx.h"
#include "Core/memory/IAllocator.h"
#include "Core/memory/MallocAllocator.h"
#include "Core/memory/LinearAllocator.h"

#define DESIRE_FRAME_ALLOCATOR_SIZE	4 * 1024 * 1024

void* IAllocator::AlignForward(void *ptr, size_t alignment)
{
	uintptr_t p = (uintptr_t)ptr;
	const size_t mod = p % alignment;
	if(mod != 0)
	{
		p += (alignment - mod);
	}
	return (void*)p;
}

IAllocator& IAllocator::GetDefaultAllocator()
{
	static MallocAllocator defaultAllocator;
	return defaultAllocator;
}

IAllocator& IAllocator::GetFrameAllocator()
{
	static std::unique_ptr<char[]> data = std::make_unique<char[]>(DESIRE_FRAME_ALLOCATOR_SIZE);
	static LinearAllocator frameAllocator(data.get(), data.get() + DESIRE_FRAME_ALLOCATOR_SIZE);
	return frameAllocator;
}

void IAllocator::ResetFrameAllocator()
{
	static_cast<LinearAllocator&>(GetFrameAllocator()).Reset();
}
