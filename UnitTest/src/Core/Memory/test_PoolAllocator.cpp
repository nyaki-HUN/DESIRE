#include "stdafx.h"
#include "Engine/Core/Memory/PoolAllocator.h"

class TestClass
{
public:
	TestClass()
		: value(123)
	{
	}

	uint32_t value;
};

TEST_CASE("PoolAllocator", "[Core][memory]")
{
	PoolAllocator<TestClass, 10> a;

	TestClass* tmp[10];
	for(uint32_t i = 0; i < 10; ++i)
	{
		tmp[i] = a.Alloc();
		REQUIRE(tmp[i] != nullptr);
		CHECK(tmp[i]->value == 123);
	}

	TestClass* pElementAllocatedWhenPoolIsEmpty = a.Alloc();

	a.Free(pElementAllocatedWhenPoolIsEmpty);
	pElementAllocatedWhenPoolIsEmpty = nullptr;

	for(uint32_t i = 0; i < 10; ++i)
	{
		a.Free(tmp[i]);
		tmp[i] = nullptr;
	}
}
