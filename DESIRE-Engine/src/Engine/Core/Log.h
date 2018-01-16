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

private:
	static Log::LogFunction_t userDefinedLogFunction;
};

// Log helper macros
#define LOG_ERROR(format, ...)					Log::LogWithFormat(__FILE__, __LINE__, "ERR", format, __VA_ARGS__)
#define LOG_WARNING(format, ...)				Log::LogWithFormat(__FILE__, __LINE__, "WRN", format, __VA_ARGS__)
#define LOG_MESSAGE(format, ...)				Log::LogWithFormat(__FILE__, __LINE__, "MSG", format, __VA_ARGS__)

#if defined(_DEBUG)
	#define LOG_DEBUG(format, ...)				Log::LogWithFormat(__FILE__, __LINE__, "DBG", format, __VA_ARGS__)
#else
	#define LOG_DEBUG(format, ...)
#endif
