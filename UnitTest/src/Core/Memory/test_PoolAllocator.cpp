#include "stdafx.h"
#include "Engine/Core/Memory/PoolAllocator.h"

class TestClass
{
public:
	TestClass()
		: value(123)
	{

	}

	int value;
};

TEST_CASE("PoolAllocator", "[Core][memory]")
{
	PoolAllocator<TestClass, 10> a;

	const size_t numAllocBegin = globalMemoryAllocationCount;

	TestClass* tmp[10];
	for(int i = 0; i < 10; ++i)
	{
		tmp[i] = a.Alloc();
		REQUIRE(tmp[i] != nullptr);
		CHECK(tmp[i]->value == 123);
	}

	CHECK_NO_ALLOCATION_SINCE(numAllocBegin);

	TestClass* elementAllocatedWhenPoolIsEmpty = a.Alloc();

	a.Free(elementAllocatedWhenPoolIsEmpty);
	elementAllocatedWhenPoolIsEmpty = nullptr;

	for(int i = 0; i < 10; ++i)
	{
		a.Free(tmp[i]);
		tmp[i] = nullptr;
	}
}
