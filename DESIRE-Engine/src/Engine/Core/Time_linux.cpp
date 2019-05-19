#include "Engine/stdafx.h"

#if defined(DESIRE_PLATFORM_LINUX)

#include "Engine/Core/Time.h"

#include <sys/time.h>

uint64_t Time::GetMicroTime()
{
	timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	uint64_t time = (uint64_t)ts.tv_sec * 1000000u + ts.tv_nsec / 1000u;

	static uint64_t startTime = time;

	return time - startTime;
}

void Time::GetGMTime(const time_t& time, tm *timeInfo)
{
	gmtime_r(&time, timeInfo);
}

void Time::GetLocalTime(const time_t& time, tm *timeInfo)
{
	localtime_r(&time, timeInfo);
}

#endif	// #if defined(DESIRE_PLATFORM_LINUX)