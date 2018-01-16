#include "Core/ANDROID/ANDROIDLoggerPolicies.h"

#include <android/log.h>

void LogCatOutputPolicy::Process(const Log::LogData& logData)
{
	android_LogPriority prio = ANDROID_LOG_DEBUG;
	switch(logData.logType[0])
	{
		case 'E':	prio = ANDROID_LOG_ERROR; break;
		case 'W':	prio = ANDROID_LOG_WARN; break;
		case 'M':	prio = ANDROID_LOG_INFO; break;
	}

	__android_log_print(prio, "DESIRE", "[%s] %s\n", logData.logType, logData.message); break;
}
