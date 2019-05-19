#include "Engine/stdafx.h"
#include "Engine/Core/Time.h"
#include "Engine/Core/String/WritableString.h"

#include <chrono>

void Time::GetDateTimeString_ISO8601(WritableString& outString)
{
	const std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	const time_t time = std::chrono::system_clock::to_time_t(now);
	tm timeInfo;
	GetGMTime(time, &timeInfo);

	outString.Sprintf("%04d-%02d-%02dT%02d:%02d:%02dZ", timeInfo.tm_year + 1900, timeInfo.tm_mon + 1, timeInfo.tm_mday, timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec);
}
