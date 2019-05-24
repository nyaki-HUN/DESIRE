#include "Engine/stdafx.h"
#include "Engine/Core/Memory/Allocator.h"
#include "Engine/Core/Memory/MallocAllocator.h"
#include "Engine/Core/Memory/LinearAllocator.h"

Allocator& Allocator::GetDefaultAllocator()
{
	static MallocAllocator s_defaultAllocator;
	return s_defaultAllocator;
}

Allocator& Allocator::GetScratchAllocator()
{
	static constexpr size_t kScratchAllocatorSize = 10 * 1024 * 1024;
	static std::unique_ptr<char[]> data = std::make_unique<char[]>(kScratchAllocatorSize);
	static LinearAllocator s_scratchAllocator(data.get(), kScratchAllocatorSize);
	return s_scratchAllocator;
}

void Allocator::ResetScratchAllocator()
{
	static_cast<LinearAllocator&>(GetScratchAllocator()).Reset();
}
