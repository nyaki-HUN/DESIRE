#include "Engine/stdafx.h"
#include "Engine/Core/Log/LoggerPolicies.h"
#include "Engine/Core/Time.h"
#include "Engine/Core/FS/FileSystem.h"
#include "Engine/Core/FS/IWriteFile.h"
#include "Engine/Core/String/StackString.h"

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

	const std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	tm timeInfo;
	Time::GetLocalTime(std::chrono::system_clock::to_time_t(now), &timeInfo);

	StackString<Log::kMaxMessageLength> strLogLine;
	strLogLine.Sprintf("%02d:%02d:%02d [%s] %s\n", timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec, logData.logType, logData.message);

	logFile->WriteBuffer(strLogLine.Str(), strLogLine.Length());
}