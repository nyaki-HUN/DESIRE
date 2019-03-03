#include "stdafx.h"
#include "Engine/Utils/JsonDataTranslator.h"

struct TestData
{
	int testInt;
	float testFloat;
	bool testBool;
	DynamicString testString;

	std::vector<int> testIntArray;
	std::vector<float> testFloatArray;
	std::vector<DynamicString> testStringArray;
};

TEST_CASE("JsonDataTranslator", "[Utils]")
{
	JsonDataTranslator<TestData> translator;
	translator.Add("intValue", &TestData::testInt)
		.Add("floatValue", &TestData::testFloat)
		.Add("boolValue", &TestData::testBool)
		.Add("stringValue", &TestData::testString)
		.Add("intArray", &TestData::testIntArray)
		.Add("floatArray", &TestData::testFloatArray)
		.Add("stringArray", &TestData::testStringArray);

	TestData data = {};

	SECTION("Test with missing values")
	{
		rapidjson::Document json(rapidjson::kObjectType);
		json.AddMember("intValue", 123, json.GetAllocator());
		json.AddMember("stringValue", "ASD", json.GetAllocator());

		rapidjson::Value jsonIntArray(rapidjson::kArrayType);
		jsonIntArray.PushBack(123, json.GetAllocator());
		json.AddMember("intArray", jsonIntArray, json.GetAllocator());

		rapidjson::Value jsonFloatArray(rapidjson::kArrayType);
		jsonFloatArray.PushBack(123.0f, json.GetAllocator());
		jsonFloatArray.PushBack(9999.0f, json.GetAllocator());
		jsonFloatArray.PushBack(0.0f, json.GetAllocator());
		json.AddMember("floatArray", jsonFloatArray, json.GetAllocator());

		rapidjson::Value jsonStringArray(rapidjson::kArrayType);
		jsonStringArray.PushBack("element0", json.GetAllocator());
		jsonStringArray.PushBack("element1", json.GetAllocator());
		json.AddMember("stringArray", jsonStringArray, json.GetAllocator());

		translator.TranslateJson(json, &data);

		CHECK(data.testInt == 123);
		CHECK(data.testString == "ASD");
		REQUIRE(data.testIntArray.size() == 1);
		CHECK(data.testIntArray[0] == 123);
		REQUIRE(data.testFloatArray.size() == 3);
		CHECK(data.testFloatArray[0] == Approx(123.0f));
		CHECK(data.testFloatArray[1] == Approx(9999.0f));
		CHECK(data.testFloatArray[2] == Approx(0.0f));
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
