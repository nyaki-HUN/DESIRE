#include "Engine/stdafx.h"

#if DESIRE_PLATFORM_LINUX

#include "Engine/Core/Time.h"

#include <sys/time.h>

uint64_t Time::GetMicroTime()
{
	timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	uint64_t time = static_cast<uint64_t>(ts.tv_sec) * 1000000u + ts.tv_nsec / 1000u;

	static uint64_t startTime = time;

	return time - startTime;
}

void Time::GetGMTime(const time_t& time, tm* pTimeInfo)
{
	gmtime_r(&time, pTimeInfo);
}

void Time::GetLocalTime(const time_t& time, tm* pTimeInfo)
{
	localtime_r(&time, pTimeInfo);
}

#endif	// #if DESIRE_PLATFORM_LINUX
