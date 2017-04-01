#include "Core/Timer.h"

#include <mach/mach_time.h>

double GetTicksPerMicroSecond()
{
	mach_timebase_info_data_t timebaseInfo;
	mach_timebase_info(&timebaseInfo);
	double ticksPerMicroSecond = (double)timebaseInfo.denom / (double)timebaseInfo.numer;	// ticks/nanosecond
	ticksPerMicroSecond *= 1000.0;															// ticks/microsecond
	return ticksPerMicroSecond;
}

uint64_t Timer::GetMicroTime()
{
	uint64_t time = mach_absolute_time();

	static uint64_t startTime = time;
	static double ticksPerMicroSecond = GetTicksPerMicroSecond();

	return (uint64_t)((double)(time - startTime) / ticksPerMicroSecond);
}
