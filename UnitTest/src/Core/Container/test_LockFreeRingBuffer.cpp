#include "stdafx.h"
#include "Engine/Core/Container/LockFreeRingBuffer.h"

TEST_CASE("LockFreeRingBuffer", "[Core]")
{
	LockFreeRingBuffer<uint32_t, 10> ringBuffer;

	std::atomic<bool> thread2Running = true;
	std::thread thread2([&ringBuffer, &thread2Running]()
	{
		uint32_t counter = 0;
		while(thread2Running || !ringBuffer.empty())
		{
			uint32_t num;
			if(ringBuffer.pop(num))
			{
				CHECK(num == counter);
				counter++;
			}
		}

		CHECK(counter == 15);
	});

	uint32_t i = 0;
	while(i < 15)
	{
		bool successful = ringBuffer.push(i);
		if(!successful)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			continue;
		}

		i++;
	}

	thread2Running = false;
	thread2.join();

	CHECK(ringBuffer.empty());
}
