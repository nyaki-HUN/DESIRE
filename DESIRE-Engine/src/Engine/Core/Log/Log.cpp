#include "Engine/stdafx.h"
#include "Engine/Core/Log/Log.h"

#include "Engine/Core/Log/Logger.h"
#include "Engine/Core/Log/LoggerPolicies.h"

Log::LogFunction_t s_userDefinedLogFunction = nullptr;

void Log::LogWithData(const LogData& logData)
{
	if(s_userDefinedLogFunction != nullptr)
	{
		s_userDefinedLogFunction(logData);
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

void Log::LogWithFormat(const char* pFilename, int32_t line, const char* pLogType, const char* pFormat, ...)
{
	LogData logData = {};
	logData.pFilename = pFilename;
	logData.line = line;
	logData.pLogType = pLogType;

	va_list args;
	va_start(args, pFormat);
	vsnprintf(logData.message, sizeof(logData.message), pFormat, args);
	va_end(args);

	Log::LogWithData(logData);
}

void Log::SetUserDefinedLogFunction(LogFunction_t func)
{
	s_userDefinedLogFunction = func;
}
