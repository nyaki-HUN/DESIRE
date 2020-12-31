#include "stdafx.h"
#undef ASSERT		// Need to undefine to bypass ASSERT in HashedStringMap::Insert() function
#define ASSERT(COND)
#include "Engine/Core/Container/HashedStringMap.h"

TEST_CASE("HashedStringMap", "[Core]")
{
	HashedStringMap<int32_t> map;
	CHECK(map.IsEmpty());
	map.Insert("asdasdasd", 100);
	CHECK_FALSE(map.IsEmpty());

	SECTION("Insert()")
	{
		map.Insert("one", 1);
		map.Insert("two", 2);
		map.Insert("three", 3);
		map.Insert("four", 4);

		CHECK(map.Size() == 5);
	}

	SECTION("Insert() fail")
	{
		map.Insert("one", 1);
		map.Insert("one", 999);

		REQUIRE(map.Find("one") != nullptr);
		CHECK(*map.Find("one") == 1);
	}

	SECTION("Find()")
	{
		REQUIRE(map.Find("asdasdasd") != nullptr);
		CHECK(*map.Find("asdasdasd") == 100);

		CHECK(map.Find("not_added") == nullptr);
	}
}
