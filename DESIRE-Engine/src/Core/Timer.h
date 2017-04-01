#pragma once

#include "Core/Singleton.h"

class Timer
{
	DESIRE_DECLARE_SINGLETON(Timer)

public:
	void Update();

	uint64_t GetTimeMicroSec() const;
	uint32_t GetTimeMilliSec() const;
	float GetTimeSec() const;

	uint64_t GetMicroDelta() const;
	uint32_t GetMilliDelta() const;
	float GetSecDelta() const;

	static uint64_t GetMicroTime();

private:
	uint64_t microSec;
	uint64_t microDelta;
	uint32_t milliSec;
	uint32_t milliDelta;
	float sec;
	float secDelta;
};
