#include "Engine/stdafx.h"

#if DESIRE_PLATFORM_IOS

#include "Engine/Core/Time.h"

#include <mach/mach_time.h>

static double GetTicksPerMicroSecond()
{
	mach_timebase_info_data_t timebaseInfo;
	mach_timebase_info(&timebaseInfo);
	double ticksPerMicroSecond = (double)timebaseInfo.denom / (double)timebaseInfo.numer;	// ticks/nanosecond
	ticksPerMicroSecond *= 1000.0;															// ticks/microsecond
	return ticksPerMicroSecond;
}

uint64_t Time::GetMicroTime()
{
	const uint64_t time = mach_absolute_time();

	static const uint64_t startTime = time;
	static const double ticksPerMicroSecond = GetTicksPerMicroSecond();

	return static_cast<uint64_t>((double)(time - startTime) / ticksPerMicroSecond);
}

void Time::GetGMTime(const time_t& time, tm *timeInfo)
{
	gmtime_r(&time, timeInfo);
}

void Time::GetLocalTime(const time_t& time, tm *timeInfo)
{
	localtime_r(&time, timeInfo);
}

#endif	// #if DESIRE_PLATFORM_IOS
