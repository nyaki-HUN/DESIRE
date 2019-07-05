#include "stdafx.h"
#include "Engine/Core/Memory/StdAllocatorWrapper.h"

TEST_CASE("StdAllocatorWrapper", "[Core][memory]")
{
	MemorySystem::ResetScratchAllocator();
	DESIRE_ALLOCATOR_SCOPE(MemorySystem::GetScratchAllocator());

	std::vector<int, StdAllocatorWrapper<int>> container;
	container.push_back(123);
	container.push_back(9999);
}
