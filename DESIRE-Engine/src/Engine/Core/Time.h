#pragma once

class WritableString;
struct tm;

class Time
{
public:
	static uint64_t GetMicroTime();

	static void GetGMTime(const time_t& time, tm *timeInfo);
	static void GetLocalTime(const time_t& time, tm *timeInfo);

	// Return the current UTC time as a string, in ISO8601 format, which is: YYYY-MM-DDTHH:MM:SSZ
	static void GetDateTimeString_ISO8601(WritableString& outString);
};
