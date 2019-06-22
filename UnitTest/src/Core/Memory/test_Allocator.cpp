#include "stdafx.h"
#include "Engine/Core/Memory/Allocator.h"

TEST_CASE("Allocator", "[Core][memory]")
{
	Allocator& defaultAllocator = Allocator::GetDefaultAllocator();
	Allocator& scratchAllocator = Allocator::GetScratchAllocator();
}

TEST_CASE("DESIRE_ALLOCATOR_NEW | DESIRE_ALLOCATOR_DELETE", "[Core][memory]")
{
	class TestClass
	{
	public:
		TestClass(int value = -1)
			: value(value)
		{

		}

		~TestClass()
		{
			value = 0;
		}

		int value;
	};

	Allocator::ResetScratchAllocator();
	Allocator& a = Allocator::GetScratchAllocator();

	const size_t numAllocBegin = globalMemoryAllocationCount;

	TestClass* ptr1 = new(a.Alloc(sizeof(TestClass))) TestClass();
	TestClass* ptr2 = new(a.Alloc(sizeof(TestClass))) TestClass(123);

	CHECK_NO_ALLOCATION_SINCE(numAllocBegin);

	// Check if the constructor has run
	CHECK(ptr1->value == -1);
	CHECK(ptr2->value == 123);

	ptr1->~TestClass();
	ptr2->~TestClass();
	a.Free(ptr1);
	a.Free(ptr2);
}
