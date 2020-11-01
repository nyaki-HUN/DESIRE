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
	uint64_t m_microSec = 0;
	uint64_t m_microDelta = 0;
	uint32_t m_milliSec = 0;
	uint32_t m_milliDelta = 0;
	float m_sec = 0.0f;
	float m_secDelta = 0.0f;
};
