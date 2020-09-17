#pragma once

class WritableString;

struct tm;

class Time
{
public:
	static uint64_t GetMicroTime();

	static void GetGMTime(const time_t& time, tm* pTimeInfo);
	static void GetLocalTime(const time_t& time, tm* pTimeInfo);

	// Return the current UTC date and time as a string, in ISO 8601 format, which is: YYYY-MM-DDTHH:MM:SSZ
	static void GetDateTimeString_ISO8601(WritableString& outString);
};
