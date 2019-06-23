#include "stdafx.h"
#include "Engine/Core/Memory/Allocator.h"

TEST_CASE("Allocator", "[Core][memory]")
{
	Allocator& a = Allocator::GetDefaultAllocator();

	SECTION("Alloc/Realloc/Free")
	{
		void* ptr = a.Alloc(10);
		memcpy(ptr, "0123456789", 10);
		a.Realloc(ptr, 20);
		CHECK(memcmp(ptr, "0123456789", 10) == 0);
		a.Realloc(ptr, 5);
		CHECK(memcmp(ptr, "01234", 5) == 0);
		a.Free(ptr);
	}
}
