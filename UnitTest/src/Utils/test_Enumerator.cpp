#include "stdafx.h"
#include "Utils/Enumerator.h"

TEST_CASE("Enumerator", "[Utils]")
{
	enum class EMyEnum
	{
		VALUE_0,
		VALUE_1,
		VALUE_2,
		NUM
	};

	std::vector<size_t> enumeratedIntValues;
	std::vector<EMyEnum> enumeratedEnumValues;

	for(auto i : Enumerator<EMyEnum>())
	{
		enumeratedIntValues.push_back(i);
		enumeratedEnumValues.push_back(i);
	}

	REQUIRE(enumeratedIntValues.size() == enumeratedEnumValues.size());
	for(size_t i = 0; i < enumeratedIntValues.size(); ++i)
	{
		CHECK(i == enumeratedIntValues[i]);
		CHECK(static_cast<EMyEnum>(i) == enumeratedEnumValues[i]);
	}
}
