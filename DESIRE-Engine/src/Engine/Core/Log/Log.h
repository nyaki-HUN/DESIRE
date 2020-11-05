#pragma once

struct LogData
{
	static constexpr size_t kMaxMessageLength = 2048;

	const char* pFilename;
	int line;
	const char* pLogType;
	char message[kMaxMessageLength];
};

class Log
{
public:
	typedef void(*LogFunction_t)(const LogData& logData);

	static void LogWithData(const LogData& logData);
	static void LogWithFormat(const char* pFilename, int line, const char* pLogType, const char* pFormat, ...);
	static void SetUserDefinedLogFunction(LogFunction_t func);
};

// Log helper macros
#define LOG_ERROR(format, ...)					Log::LogWithFormat(__FILE__, __LINE__, "ERR", format, __VA_ARGS__)
#define LOG_WARNING(format, ...)				Log::LogWithFormat(__FILE__, __LINE__, "WRN", format, __VA_ARGS__)
#define LOG_MESSAGE(format, ...)				Log::LogWithFormat(__FILE__, __LINE__, "MSG", format, __VA_ARGS__)

#if defined(_DEBUG)
	#define LOG_DEBUG(format, ...)				Log::LogWithFormat(__FILE__, __LINE__, "DBG", format, __VA_ARGS__)
#else
	#define LOG_DEBUG(format, ...)				(void)(format, __VA_ARGS__)
#endif
