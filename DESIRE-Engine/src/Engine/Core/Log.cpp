#include "Engine/stdafx.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/Logger.h"

#if defined(DESIRE_PLATFORM_WINDOWS)
	#include "Engine/Core/WINDOWS/WINDOWSLoggerPolicies.h"
#elif defined(DESIRE_PLATFORM_IOS)
	#include "Engine/Core/IOS/IOSLoggerPolicies.h"
#elif defined(DESIRE_PLATFORM_ANDROID)
	#include "Engine/Core/ANDROID/ANDROIDLoggerPolicies.h"
#endif

Log::LogFunction_t Log::userDefinedLogFunction = nullptr;

void Log::LogWithData(const Log::LogData& logData)
{
	if(userDefinedLogFunction != nullptr)
	{
		userDefinedLogFunction(logData);
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
	Log::LogData logData;
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
	userDefinedLogFunction = func;
}
