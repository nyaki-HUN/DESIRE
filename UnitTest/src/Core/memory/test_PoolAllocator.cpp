#include "stdafx.h"
#include "Engine/Core/memory/PoolAllocator.h"

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

	TestClass *tmp[10];
	for(int i = 0; i < 10; ++i)
	{
		tmp[i] = a.Allocate();
		REQUIRE(tmp[i] != nullptr);
		CHECK(tmp[i]->value == 123);
	}

	for(int i = 0; i < 10; ++i)
	{
		a.Deallocate(tmp[i]);
		tmp[i] = nullptr;
	}
}
