#pragma once

#include "Engine/Core/Log/Log.h"

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

	inline void Log(const Log::LogData& logData)
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
