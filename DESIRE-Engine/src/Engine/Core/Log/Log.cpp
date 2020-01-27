#include "Engine/stdafx.h"
#include "Engine/Core/Log/Log.h"
#include "Engine/Core/Log/Logger.h"
#include "Engine/Core/Log/LoggerPolicies.h"

Log::LogFunction_t Log::userDefinedLogFunction = nullptr;

void Log::LogWithData(const LogData& logData)
{
	if(userDefinedLogFunction != nullptr)
	{
		userDefinedLogFunction(logData);
	}
	else
	{
		// Default logging
#if DESIRE_PLATFORM_WINDOWS
		static Logger<NoFilterPolicy, VisualStudioOutputPolicy> ideLogger;
		ideLogger.Log(logData);

		static Logger<NoFilterPolicy, ConsoleWindowOutputPolicy> consoleLogger;
#elif DESIRE_PLATFORM_IOS
		static Logger<NoFilterPolicy, NSLogOutputPolicy> consoleLogger;
#elif DESIRE_PLATFORM_ANDROID
		static Logger<NoFilterPolicy, LogCatOutputPolicy> consoleLogger;
#else
		static Logger<NoFilterPolicy, ConsoleOutputPolicy> consoleLogger;
#endif
		consoleLogger.Log(logData);
	}
}

void Log::LogWithFormat(const char* file, int line, const char* logType, const char* format, ...)
{
	LogData logData;
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
