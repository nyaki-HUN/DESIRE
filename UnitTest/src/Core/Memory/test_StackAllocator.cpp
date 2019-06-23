#include "stdafx.h"
#include "Engine/Core/Memory/StackAllocator.h"

TEST_CASE("StackAllocator", "[Core][memory]")
{
	StackAllocator<128> a;

	const void* memoryStart = (char*)& a + sizeof(LinearAllocator);
	const void* memoryEnd = (char*)memoryStart + (sizeof(a) - sizeof(LinearAllocator));

	SECTION("Default alignment")
	{
		const size_t numAllocBegin = globalMemoryAllocationCount;

		void* ptr1 = a.Alloc(1);
		void* ptr2 = a.Alloc(1);
		CHECK_NO_ALLOCATION_SINCE(numAllocBegin);

		CHECK(reinterpret_cast<uintptr_t>(ptr1) % Allocator::kDefaultAlignment == 0);
		CHECK(reinterpret_cast<uintptr_t>(ptr2) % Allocator::kDefaultAlignment == 0);
	}

	SECTION("Test Fallback Allocator 1/4")
	{
		const size_t numAllocBegin = globalMemoryAllocationCount;

		// ptrFB is supposed to be allocated via the fallback allocator
		void* ptr1 = a.Alloc(64);
		void* ptr2 = a.Alloc(64);
		CHECK_NO_ALLOCATION_SINCE(numAllocBegin);
		void* ptrFB = a.Alloc(10);

		CHECK((memoryStart <= ptr1 && ptr1 < memoryEnd));
		CHECK((memoryStart <= ptr2 && ptr2 < memoryEnd));
		CHECK_FALSE((memoryStart <= ptrFB && ptrFB < memoryEnd));

		a.Free(ptr1);
		a.Free(ptr2);
		a.Free(ptrFB);
	}

	SECTION("Test Fallback Allocator 2/4")
	{
		size_t numAllocBegin = globalMemoryAllocationCount;

		void* ptr1 = a.Alloc(64);
		CHECK_NO_ALLOCATION_SINCE(numAllocBegin);
		void* ptrFB = a.Alloc(100);

		numAllocBegin = globalMemoryAllocationCount;
		void* ptr2 = a.Alloc(10);
		CHECK_NO_ALLOCATION_SINCE(numAllocBegin);

		CHECK((memoryStart <= ptr1 && ptr1 < memoryEnd));
		CHECK((memoryStart <= ptr2 && ptr2 < memoryEnd));
		CHECK_FALSE((memoryStart <= ptrFB && ptrFB < memoryEnd));

		a.Free(ptr1);
		a.Free(ptr2);
		a.Free(ptrFB);
	}

	SECTION("Test Fallback Allocator 3/4")
	{
		void* ptrFB = a.Alloc(200);
		CHECK_FALSE((memoryStart <= ptrFB && ptrFB < memoryEnd));

		a.Free(ptrFB);
	}

	SECTION("Test Fallback Allocator 4/4")
	{
		// Because of the alignment the second allocations cannot be made regardless of the fact that the overall size would be only 128
		void* ptr1 = a.Alloc(113);
		void* ptrFB = a.Alloc(15);
		CHECK((memoryStart <= ptr1 && ptr1 < memoryEnd));
		CHECK_FALSE((memoryStart <= ptrFB && ptrFB < memoryEnd));

		a.Free(ptr1);
		a.Free(ptrFB);
	}
}
