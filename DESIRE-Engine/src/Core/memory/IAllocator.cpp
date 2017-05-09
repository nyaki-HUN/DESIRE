#include "stdafx.h"
#include "Core/memory/IAllocator.h"
#include "Core/memory/MallocAllocator.h"
#include "Core/memory/LinearAllocator.h"

#define DESIRE_FRAME_ALLOCATOR_SIZE	4 * 1024 * 1024

IAllocator& IAllocator::GetDefaultAllocator()
{
	static MallocAllocator defaultAllocator;
	return defaultAllocator;
}

IAllocator& IAllocator::GetFrameAllocator()
{
	static std::unique_ptr<char[]> data = std::make_unique<char[]>(DESIRE_FRAME_ALLOCATOR_SIZE);
	static LinearAllocator frameAllocator(data.get(), DESIRE_FRAME_ALLOCATOR_SIZE);
	return frameAllocator;
}

void IAllocator::ResetFrameAllocator()
{
	static_cast<LinearAllocator&>(GetFrameAllocator()).Reset();
}
