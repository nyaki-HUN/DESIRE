#pragma once

class Log
{
public:
	static const size_t MAX_MESSAGE_LENGTH = 2048;

	struct LogData
	{
		const char *file;
		int line;
		const char *logType;
		char message[MAX_MESSAGE_LENGTH];
	};

	typedef void(*LogFunction_t)(const Log::LogData& logData);

	static void LogWithData(const Log::LogData& logData);
	static void LogWithFormat(const char *file, int line, const char *logType, const char *format, ...);
	static void SetUserDefinedLogFunction(LogFunction_t func);
};

// Log helper macros
#if defined(DESIRE_DISTRIBUTION)
	#define DESIRE_LOG(type, format, ...)
#else
	#define DESIRE_LOG(type, format, ...)		Log::LogWithFormat(__FILE__, __LINE__, type, format, __VA_ARGS__)
#endif

#define LOG_ERROR(format, ...)					DESIRE_LOG("ERR", format, __VA_ARGS__)
#define LOG_WARNING(format, ...)				DESIRE_LOG("WRN", format, __VA_ARGS__)
#define LOG_MESSAGE(format, ...)				DESIRE_LOG("MSG", format, __VA_ARGS__)

#if defined(DESIRE_DEBUG)
	#define LOG_DEBUG(format, ...)				DESIRE_LOG("DBG", format, __VA_ARGS__)
#else
	#define LOG_DEBUG(format, ...)
#endif
