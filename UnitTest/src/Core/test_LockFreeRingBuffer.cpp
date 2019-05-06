#include "stdafx.h"
#include "Engine/Core/Container/LockFreeRingBuffer.h"

#include <thread>

TEST_CASE("LockFreeRingBuffer", "[Core]")
{
	LockFreeRingBuffer<int, 10> ringBuffer;

	std::atomic<bool> thread2Running = true;
	std::thread thread2([&ringBuffer, &thread2Running]()
	{
		int counter = 0;
		while(thread2Running || !ringBuffer.empty())
		{
			int num;
			if(ringBuffer.pop(num))
			{
				CHECK(num == counter);
				counter++;
			}
		}

		CHECK(counter == 15);
	});

	int i = 0;
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
