#include "stdafx.h"
#include "Core/memory/StdFrameAllocatorWrapper.h"

TEST_CASE("StdFrameAllocatorWrapper", "[Core][memory]")
{
	IAllocator::ResetFrameAllocator();

	{
		const size_t numAllocBegin = globalMemoryAllocationCount;

		std::vector<int> containerWithDefaultAllocator;
		containerWithDefaultAllocator.push_back(123);
		containerWithDefaultAllocator.push_back(9999);

		const size_t numAllocEnd = globalMemoryAllocationCount;
		CHECK(numAllocBegin != numAllocEnd);
	}

	{
		const size_t numAllocBegin = globalMemoryAllocationCount;

		std::vector<int, StdFrameAllocatorWrapper<int>> container;
		container.push_back(123);
		container.push_back(9999);

		CHECK_NO_ALLOCATION_SINCE(numAllocBegin);
	}
}
