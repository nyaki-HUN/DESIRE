#include "Engine/stdafx.h"

#if DESIRE_PLATFORM_WINDOWS

#include "Engine/Core/Time.h"
#include "Engine/Core/WINDOWS/os.h"		// we need <windows.h> for the LARGE_INTEGER type

static double GetTicksPerMicroSecond()
{
	LARGE_INTEGER qwTimerFrequency;
	QueryPerformanceFrequency(&qwTimerFrequency);											// ticks/second
	double ticksPerMicroSecond = static_cast<double>(qwTimerFrequency.QuadPart) / 1000.0;	// ticks/millisecond
	ticksPerMicroSecond /= 1000.0;															// ticks/microsecond
	return ticksPerMicroSecond;
}

uint64_t Time::GetMicroTime()
{
	LARGE_INTEGER qwTime;
	QueryPerformanceCounter(&qwTime);

	static const LARGE_INTEGER startTime = qwTime;
	static const double ticksPerMicroSecond = GetTicksPerMicroSecond();

	return static_cast<uint64_t>(static_cast<double>(qwTime.QuadPart - startTime.QuadPart) / ticksPerMicroSecond);
}

void Time::GetGMTime(const time_t& time, tm* pTimeInfo)
{
	gmtime_s(pTimeInfo, &time);
}

void Time::GetLocalTime(const time_t& time, tm* pTimeInfo)
{
	localtime_s(pTimeInfo, &time);
}

#endif	// #if DESIRE_PLATFORM_WINDOWS
