#include "Engine/stdafx.h"
#include "Engine/Core/Logger.h"
#include "Engine/Core/FS/FileSystem.h"
#include "Engine/Core/FS/IWriteFile.h"

#include <time.h>

void ConsoleOutputPolicy::Process(const Log::LogData& logData)
{
	printf("[%s] %s\n", logData.logType, logData.message);
	fflush(0);
}

FileOutputPolicy::FileOutputPolicy()
{
	logFile = FileSystem::Get()->CreateWriteFile("log.txt");
	ASSERT(logFile != nullptr);
}

void FileOutputPolicy::Process(const Log::LogData& logData)
{
	if(logFile == nullptr)
	{
		return;
	}

	char strTime[10];
#if defined(DESIRE_PLATFORM_WINDOWS)
	_strtime_s(strTime);
#else
	time_t now = time(nullptr);
	strftime(strTime, sizeof(strTime), "%T", localtime(&now));
#endif

	char logLine[Log::kMaxMessageLength];
	int len = snprintf(logLine, Log::kMaxMessageLength, "%s [%s] % s\n", strTime, logData.logType, logData.message);
	if(len > 0)
	{
		const size_t numCharsToWrite = std::min((size_t)len, Log::kMaxMessageLength);
		logFile->WriteBuffer(logLine, numCharsToWrite);
	}
}
