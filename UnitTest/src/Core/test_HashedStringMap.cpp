#include "stdafx.h"
#undef ASSERT		// Need to undefine to bypass ASSERT in HashedStringMap::Insert() function
#define ASSERT(COND)	((void)0)
#include "Engine/Core/HashedStringMap.h"

TEST_CASE("HashedStringMap", "[Core]")
{
	HashedStringMap<int> map;
	map.Insert("asdasdasd", 100);

	SECTION("Insert()")
	{
		map.Insert("one", 1);
		map.Insert("two", 2);
		map.Insert("three", 3);
		map.Insert("four", 4);

		// Hack to get access to elements
		std::vector<std::pair<HashedString, int>>& elements = *reinterpret_cast<std::vector<std::pair<HashedString, int>>*>(&map);

		CHECK(elements.size() == 5);
		int totalValue = elements[0].second;
		for(size_t i = 1; i < elements.size(); ++i)
		{
			totalValue += elements[i].second;
			CHECK(elements[i - 1].first < elements[i].first);
		}

		CHECK(totalValue == 100 + 1 + 2 + 3 + 4);
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
