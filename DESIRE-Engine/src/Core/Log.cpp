#include "stdafx.h"
#include "Core/Log.h"
#include "Core/Logger.h"

#if defined(DESIRE_PLATFORM_WINDOWS)
	#include "Core/WINDOWS/WINDOWSLoggerPolicies.h"
#elif defined(DESIRE_PLATFORM_IOS)
	#include "Core/IOS/IOSLoggerPolicies.h"
#elif defined(DESIRE_PLATFORM_ANDROID)
	#include "Core/ANDROID/ANDROIDLoggerPolicies.h"
#endif

#include <stdarg.h>

Log::LogFunction_t desireUserDefinedLogFunction = nullptr;

void Log::LogWithData(const Log::SLogData& logData)
{
	if(desireUserDefinedLogFunction != nullptr)
	{
		desireUserDefinedLogFunction(logData);
	}
	else
	{
		// Default logging
#if defined(DESIRE_PLATFORM_WINDOWS)
		static Logger<NoFilterPolicy, VisualStudioOutputPolicy> ideLogger;
		ideLogger.Log(logData);

		static Logger<NoFilterPolicy, WIN32ConsoleOutputPolicy> consoleLogger;
#elif defined(DESIRE_PLATFORM_IOS)
		static Logger<NoFilterPolicy, NSLogOutputPolicy> consoleLogger;
#elif defined(DESIRE_PLATFORM_ANDROID)
		static Logger<NoFilterPolicy, LogCatOutputPolicy> consoleLogger;
#else
		static Logger<NoFilterPolicy, ConsoleOutputPolicy> consoleLogger;
#endif
		consoleLogger.Log(logData);
	}
}

void Log::LogWithFormat(const char *file, int line, const char *logType, const char *format, ...)
{
	Log::SLogData logData;
	logData.file = file;
	logData.line = line;
	logData.logType = logType;

	va_list args;
	va_start(args, format);
	vsnprintf(logData.message, sizeof(logData.message), format, args);
	va_end(args);

	Log::LogWithData(logData);
}

void Log::SetUserDefinedLogFunction(LogFunction_t func)
{
	desireUserDefinedLogFunction = func;
}
