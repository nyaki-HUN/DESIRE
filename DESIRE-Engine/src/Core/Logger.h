#pragma once

#include "Core/Log.h"
#include "Core/fs/FilePtr_fwd.h"

// --------------------------------------------------------------------------------------------------------------------
//	This is a very generic logger class which passes on the task of filtering, formatting and outputting messages to
//	certain policies, which are handed down to the implementation via template parameters.
// --------------------------------------------------------------------------------------------------------------------

template<typename FilterPolicy, typename OutputPolicy>
class Logger
{
public:
	Logger() {}
	~Logger() {}

	inline void Log(const Log::SLogData& logData)
	{
		if(filterPolicy.Filter(logData))
		{
			outputPolicy.Process(logData);
		}
	}

private:
	FilterPolicy filterPolicy;
	OutputPolicy outputPolicy;
};

// --------------------------------------------------------------------------------------------------------------------
//	Default filter policies
// --------------------------------------------------------------------------------------------------------------------

struct NoFilterPolicy
{
	inline bool Filter(const Log::SLogData& /*logData*/)
	{
		return true;
	}
};

template<class Policy1, class Policy2>
class CombinedFilterPolicy
{
	inline bool Filter(const Log::SLogData& logData)
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
	void Process(const Log::SLogData& logData);
};

struct FileOutputPolicy
{
	FileOutputPolicy();
	void Process(const Log::SLogData& logData);

	WriteFilePtr logFile;
};
