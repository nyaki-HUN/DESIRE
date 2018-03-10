#include "Engine/stdafx.h"
#include "Engine/Core/Timer.h"

Timer::Timer()
{
	Update();
}

Timer::~Timer()
{

}

void Timer::Update()
{
	microDelta = microSec;
	microSec = GetMicroTime();
	microDelta = microSec - microDelta;

	milliSec = (uint32_t)(microSec / 1000);
	milliDelta = (uint32_t)(microDelta / 1000);

	sec = (float)milliSec / 1000.0f;
	secDelta = (float)milliDelta / 1000.0f;
}

uint64_t Timer::GetTimeMicroSec() const
{
	return microSec;
}

uint32_t Timer::GetTimeMilliSec() const
{
	return milliSec;
}

float Timer::GetTimeSec() const
{
	return sec;
}

uint64_t Timer::GetMicroDelta() const
{
	return microDelta;
}

uint32_t Timer::GetMilliDelta() const
{
	return milliDelta;
}

float Timer::GetSecDelta() const
{
	return secDelta;
}
