#include "Engine/stdafx.h"
#include "Engine/Core/Memory/Allocator.h"
#include "Engine/Core/Memory/MallocAllocator.h"
#include "Engine/Core/Memory/LinearAllocator.h"

#define DESIRE_FRAME_ALLOCATOR_SIZE	4 * 1024 * 1024

Allocator& Allocator::GetDefaultAllocator()
{
	static MallocAllocator defaultAllocator;
	return defaultAllocator;
}

Allocator& Allocator::GetFrameAllocator()
{
	static std::unique_ptr<char[]> data = std::make_unique<char[]>(DESIRE_FRAME_ALLOCATOR_SIZE);
	static LinearAllocator frameAllocator(data.get(), DESIRE_FRAME_ALLOCATOR_SIZE);
	return frameAllocator;
}

void Allocator::ResetFrameAllocator()
{
	static_cast<LinearAllocator&>(GetFrameAllocator()).Reset();
}
