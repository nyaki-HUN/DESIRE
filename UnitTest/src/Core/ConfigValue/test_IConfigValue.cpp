#include "stdafx.h"
#include "Core/ConfigValue/ConfigValueInt.h"
#include "Core/ConfigValue/ConfigValueFloat.h"

TEST_CASE("IConfigValue", "[Core]")
{
	ConfigValueInt intVal("test_int", "Integer test value", 10, -50, 100);
	ConfigValueFloat floatVal("test_float", "Float test value", 0.0f, 0.0f, 10.0f);

	SECTION("FindConfigValue()")
	{
		CHECK(&intVal == IConfigValue::FindConfigValue("test_int"));
		CHECK(&floatVal == IConfigValue::FindConfigValue("test_float"));

		CHECK(nullptr == IConfigValue::FindConfigValue("tmp"));
		{
			ConfigValueInt tempVal("tmp", "Temp value", 0);
			CHECK(&tempVal == IConfigValue::FindConfigValue("tmp"));
		}
		CHECK(nullptr == IConfigValue::FindConfigValue("tmp"));
	}

	SECTION("operator =")
	{
		CHECK(intVal == 10);
		CHECK(floatVal == 0.0f);

		intVal = 10;
		CHECK(intVal == 10);
		floatVal = 5.0f;
		CHECK_FLOATS(floatVal, 5.0f);

		// Clamp to [min, max] range
		intVal = -100;
		CHECK(intVal == -50);
		floatVal = -10.0f;
		CHECK(floatVal == 0.0f);
		intVal = INT32_MAX;
		CHECK(intVal == 100);
		floatVal = 100.0f;
		CHECK_FLOATS(floatVal, 10.0f);
	}
}
