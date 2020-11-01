#pragma once

#include "Engine/Core/FS/FilePtr_fwd.h"

struct LogData;

// --------------------------------------------------------------------------------------------------------------------
//	Default filter policies
// --------------------------------------------------------------------------------------------------------------------

struct NoFilterPolicy
{
	inline bool Filter(const LogData& logData)
	{
		DESIRE_UNUSED(logData);
		return true;
	}
};

template<class Policy1, class Policy2>
class CombinedFilterPolicy
{
	inline bool Filter(const LogData& logData)
	{
		return (m_policy1.Filter(logData) && m_policy2.Filter(logData));
	}

private:
	Policy1 m_policy1;
	Policy2 m_policy2;
};

// --------------------------------------------------------------------------------------------------------------------
//	Default output policies
// --------------------------------------------------------------------------------------------------------------------

struct ConsoleOutputPolicy
{
	void Process(const LogData& logData);
};

struct FileOutputPolicy
{
	FileOutputPolicy();
	void Process(const LogData& logData);

	WriteFilePtr logFile;
};

// --------------------------------------------------------------------------------------------------------------------
//	Platform specific output policies
// --------------------------------------------------------------------------------------------------------------------

#if DESIRE_PLATFORM_WINDOWS

struct ConsoleWindowOutputPolicy
{
	ConsoleWindowOutputPolicy();
	void Process(const LogData& logData);
};

struct VisualStudioOutputPolicy
{
	void Process(const LogData& logData);
};

#elif DESIRE_PLATFORM_ANDROID

struct LogCatOutputPolicy
{
	void Process(const LogData& logData);
};

#elif DESIRE_PLATFORM_IOS

struct NSLogOutputPolicy
{
	void Process(const LogData& logData);
};

struct XcodeOutputPolicy
{
	void Process(const LogData& logData);
};

#endif
