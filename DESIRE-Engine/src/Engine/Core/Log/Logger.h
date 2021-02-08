#pragma once

struct LogData;

// --------------------------------------------------------------------------------------------------------------------
//	This is a very generic logger class which passes on the task of filtering, formatting and outputting messages to
//	certain policies, which are handed down to the implementation via template parameters.
// --------------------------------------------------------------------------------------------------------------------

template<typename FilterPolicy, typename OutputPolicy>
class Logger
{
public:
	void Log(const LogData& logData)
	{
		if(m_filterPolicy.Filter(logData))
		{
			m_outputPolicy.Process(logData);
		}
	}

private:
	FilterPolicy m_filterPolicy;
	OutputPolicy m_outputPolicy;
};
