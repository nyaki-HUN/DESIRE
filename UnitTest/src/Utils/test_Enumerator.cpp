#include "stdafx.h"
#include "Engine/Utils/Enumerator.h"

TEST_CASE("Enumerator", "[Utils]")
{
	enum class EMyEnum
	{
		Value0,
		Value1,
		Value2,
		Num
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
