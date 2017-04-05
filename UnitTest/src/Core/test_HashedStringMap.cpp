#include "stdafx.h"
#include "Core/HashedStringMap.h"

TEST_CASE("HashedStringMap", "[Core]")
{
	HashedStringMap<int> map;
	map.Insert("one", 1);
	map.Insert("two", 2);
	map.Insert("three", 3);
	map.Insert("four", 4);
	map.Insert("asdasdasd", 5);
	map.Insert("qweqwe", 6);

	SECTION("Insert()")
	{
		// Hack to get access to elements
		std::vector<std::pair<HashedString, int>>& elements = *reinterpret_cast<std::vector<std::pair<HashedString, int>>*>(&map);

		CHECK(elements.size() == 6);
		int totalValue = elements[0].second;
		for(size_t i = 1; i < elements.size(); ++i)
		{
			totalValue += elements[i].second;
			CHECK(elements[i - 1].first < elements[i].first);
		}

		CHECK(totalValue == 1 + 2 + 3 + 4 + 5 + 6);
	}

	SECTION("Find()")
	{
		REQUIRE(map.Find("asdasdasd") != nullptr);
		CHECK(*map.Find("asdasdasd") == 5);

		CHECK(map.Find("not_added") == nullptr);
	}
}
