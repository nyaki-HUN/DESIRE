#include "stdafx.h"
#include "Utils/JsonDataTranslator.h"

struct STestData
{
	int testInt;
	float testFloat;
	bool testBool;
	std::string testString;

	std::vector<int> testIntArray;
	std::vector<std::string> testStringArray;
};

TEST_CASE("JsonDataTranslator", "[Utils]")
{
	JsonDataTranslator<STestData> translator;
	translator.Add("intValue", &STestData::testInt)
		.Add("floatValue", &STestData::testFloat)
		.Add("boolValue", &STestData::testBool)
		.Add("stringValue", &STestData::testString)
		.Add("stringArray", &STestData::testStringArray);

	STestData data = {};

	SECTION("Test with missing values")
	{
		rapidjson::Document json(rapidjson::kObjectType);
		json.AddMember("intValue", 123, json.GetAllocator());
		json.AddMember("stringValue", "ASD", json.GetAllocator());

		rapidjson::Value jsonArray(rapidjson::kArrayType);
		jsonArray.PushBack("element0", json.GetAllocator());
		jsonArray.PushBack("element1", json.GetAllocator());
		json.AddMember("stringArray", jsonArray, json.GetAllocator());

		translator.TranslateJson(json, &data);

		CHECK(data.testInt == 123);
		CHECK(data.testString == "ASD");
		REQUIRE(data.testStringArray.size() == 2);
		CHECK(data.testStringArray[0] == "element0");
		CHECK(data.testStringArray[1] == "element1");
	}

	SECTION("Test with mixed order")
	{
		rapidjson::Document json(rapidjson::kObjectType);
		json.AddMember("stringValue", "ASD", json.GetAllocator());
		json.AddMember("intValue", 123, json.GetAllocator());
		json.AddMember("boolValue", true, json.GetAllocator());
		json.AddMember("floatValue", FLT_MAX, json.GetAllocator());

		rapidjson::Value jsonArray(rapidjson::kArrayType);
		json.AddMember("stringArray", jsonArray, json.GetAllocator());

		translator.TranslateJson(json, &data);

		CHECK(data.testInt == 123);
		CHECK(data.testFloat == FLT_MAX);
		CHECK(data.testBool == true);
		CHECK(data.testString == "ASD");
		CHECK(data.testStringArray.size() == 0);
	}
}
