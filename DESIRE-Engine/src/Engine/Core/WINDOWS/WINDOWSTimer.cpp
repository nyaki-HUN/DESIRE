#include "Engine/stdafx.h"
#include "Engine/Core/Timer.h"
#include "Engine/Core/WINDOWS/os.h"		// we need <windows.h> for the LARGE_INTEGER type

double GetTicksPerMicroSecond()
{
	LARGE_INTEGER qwTimerFrequency;
	QueryPerformanceFrequency(&qwTimerFrequency);								// ticks/second
	double ticksPerMicroSecond = (double)qwTimerFrequency.QuadPart / 1000.0;	// ticks/millisecond
	ticksPerMicroSecond /= 1000.0;												// ticks/microsecond
	return ticksPerMicroSecond;
}

uint64_t Timer::GetMicroTime()
{
	LARGE_INTEGER qwTime;
	QueryPerformanceCounter(&qwTime);

	static LARGE_INTEGER startTime = qwTime;
	static double ticksPerMicroSecond = GetTicksPerMicroSecond();

	return (uint64_t)((double)(qwTime.QuadPart - startTime.QuadPart) / ticksPerMicroSecond);
}
