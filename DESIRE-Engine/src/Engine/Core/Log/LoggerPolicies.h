#pragma once

#include "Engine/Core/FS/FilePtr_fwd.h"
#include "Engine/Core/Log/Log.h"

// --------------------------------------------------------------------------------------------------------------------
//	Default filter policies
// --------------------------------------------------------------------------------------------------------------------

struct NoFilterPolicy
{
	inline bool Filter(const Log::LogData& /*logData*/)
	{
		return true;
	}
};

template<class Policy1, class Policy2>
class CombinedFilterPolicy
{
	inline bool Filter(const Log::LogData& logData)
	{
		return (policy1.Filter(logData) && policy2.Filter(logData));
	}

private:
	Policy1 policy1;
	Policy2 policy2;
};

// --------------------------------------------------------------------------------------------------------------------
//	Default output policies
// --------------------------------------------------------------------------------------------------------------------

struct ConsoleOutputPolicy
{
	void Process(const Log::LogData& logData);
};

struct FileOutputPolicy
{
	FileOutputPolicy();
	void Process(const Log::LogData& logData);

	WriteFilePtr logFile;
};

// --------------------------------------------------------------------------------------------------------------------
//	Platform specific output policies
// --------------------------------------------------------------------------------------------------------------------

#if DESIRE_PLATFORM_WINDOWS

struct WIN32ConsoleOutputPolicy
{
	WIN32ConsoleOutputPolicy();
	void Process(const Log::LogData& logData);
};

struct VisualStudioOutputPolicy
{
	void Process(const Log::LogData& logData);
};

#elif DESIRE_PLATFORM_ANDROID

struct LogCatOutputPolicy
{
	void Process(const Log::LogData& logData);
};

#elif DESIRE_PLATFORM_IOS

struct NSLogOutputPolicy
{
	void Process(const Log::LogData& logData);
};

#endif
