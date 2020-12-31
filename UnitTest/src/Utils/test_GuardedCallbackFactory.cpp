#include "stdafx.h"
#include "Engine/Utils/GuardedCallbackFactory.h"

TEST_CASE("GuardedCallbackFactory", "[Utils]")
{
	std::function<void()> callback = nullptr;
	uint32_t x = 0;

	// Add scope to also check the case when callbackFactory is destroyed
	{
		GuardedCallbackFactory callbackFactory;

		callback = callbackFactory.CreateCallback([&x]()
		{
			x++;
		});

		callback();
		CHECK(x == 1);

		callback();
		CHECK(x == 2);
	}

	callback();
	CHECK(x == 2);
}
