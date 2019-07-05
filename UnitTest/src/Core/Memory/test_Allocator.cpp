#include "stdafx.h"
#include "Engine/Core/Memory/Allocator.h"

TEST_CASE("Allocator", "[Core][memory]")
{
	Allocator& a = Allocator::GetDefaultAllocator();

	SECTION("Alloc/Realloc/Free")
	{
		void* ptr = a.Alloc(10);
		REQUIRE(ptr != nullptr);
		memcpy(ptr, "0123456789", 10);

		ptr = a.Realloc(ptr, 20, 10);
		REQUIRE(ptr != nullptr);
		CHECK(memcmp(ptr, "0123456789", 10) == 0);
		
		ptr = a.Realloc(ptr, 5, 20);
		REQUIRE(ptr != nullptr);
		CHECK(memcmp(ptr, "01234", 5) == 0);

		a.Free(ptr, 5);
	}
}
