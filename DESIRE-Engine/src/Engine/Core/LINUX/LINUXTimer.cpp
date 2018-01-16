#include "Core/Timer.h"

#include <sys/time.h>

uint64_t Timer::GetMicroTime()
{
	timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	uint64_t time = (uint64_t)ts.tv_sec * 1000000u + ts.tv_nsec / 1000u;

	static uint64_t startTime = time;

	return time - startTime;
}
