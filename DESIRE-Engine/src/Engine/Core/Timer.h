#pragma once

class Timer
{
public:
	Timer();
	~Timer();

	void Update();

	uint64_t GetTimeMicroSec() const;
	uint32_t GetTimeMilliSec() const;
	float GetTimeSec() const;

	uint64_t GetMicroDelta() const;
	uint32_t GetMilliDelta() const;
	float GetSecDelta() const;

private:
	uint64_t microSec = 0;
	uint64_t microDelta = 0;
	uint32_t milliSec = 0;
	uint32_t milliDelta = 0;
	float sec = 0.0f;
	float secDelta = 0.0f;
};
