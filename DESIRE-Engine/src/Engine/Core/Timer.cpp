#include "Engine/stdafx.h"
#include "Engine/Core/Timer.h"

#include "Engine/Core/Time.h"

Timer::Timer()
{
	Update();
}

Timer::~Timer()
{
}

void Timer::Update()
{
	const uint64_t prevMicroSec = m_microSec;
	m_microSec = Time::GetMicroTime();
	m_microDelta = m_microSec - prevMicroSec;

	m_milliSec = static_cast<uint32_t>(m_microSec / 1000);
	m_milliDelta = static_cast<uint32_t>(m_microDelta / 1000);

	m_sec = static_cast<float>(m_milliSec) / 1000.0f;
	m_secDelta = static_cast<float>(m_milliDelta) / 1000.0f;
}

uint64_t Timer::GetTimeMicroSec() const
{
	return m_microSec;
}

uint32_t Timer::GetTimeMilliSec() const
{
	return m_milliSec;
}

float Timer::GetTimeSec() const
{
	return m_sec;
}

uint64_t Timer::GetMicroDelta() const
{
	return m_microDelta;
}

uint32_t Timer::GetMilliDelta() const
{
	return m_milliDelta;
}

float Timer::GetSecDelta() const
{
	return m_secDelta;
}
