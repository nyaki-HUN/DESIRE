#include "stdafx.h"
#include "Core/memory/IAllocator.h"

TEST_CASE("IAllocator", "[Core][memory]")
{
	IAllocator& defaultAllocator = IAllocator::GetDefaultAllocator();
	IAllocator& frameAllocator = IAllocator::GetFrameAllocator();
}

TEST_CASE("DESIRE_ALLOCATOR_NEW && DESIRE_ALLOCATOR_DELETE", "[Core][memory]")
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

	IAllocator::ResetFrameAllocator();
	IAllocator& a = IAllocator::GetFrameAllocator();

	const size_t numAllocBegin = globalMemoryAllocationCount;

	TestClass *ptr1 = DESIRE_ALLOCATOR_NEW(a, TestClass);
	TestClass *ptr2 = DESIRE_ALLOCATOR_NEW(a, TestClass, 123);

	CHECK_NO_ALLOCATION_SINCE(numAllocBegin);

	// Check if the constructor has run
	CHECK(ptr1->value == -1);
	CHECK(ptr2->value == 123);

	DESIRE_ALLOCATOR_DELETE(a, TestClass, ptr1);
	DESIRE_ALLOCATOR_DELETE(a, TestClass, ptr2);

	// Check if the destructor has run (the memory is not freed yet at this point)
	CHECK(ptr1->value == 0);
	CHECK(ptr2->value == 0);
}
