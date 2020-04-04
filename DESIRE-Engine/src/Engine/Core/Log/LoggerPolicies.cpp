#include "Engine/stdafx.h"
#include "Engine/Core/Log/LoggerPolicies.h"

#include "Engine/Core/FS/FileSystem.h"
#include "Engine/Core/FS/IWriteFile.h"
#include "Engine/Core/String/StackString.h"
#include "Engine/Core/Time.h"

void ConsoleOutputPolicy::Process(const LogData& logData)
{
	printf("[%s] %s\n", logData.logType, logData.message);
	fflush(0);
}

FileOutputPolicy::FileOutputPolicy()
{
	StackString<DESIRE_MAX_PATH_LEN> filenameWithPath = FileSystem::Get()->GetAppDirectory();
	filenameWithPath += "log.txt";

	logFile = FileSystem::Get()->CreateWriteFileNative(filenameWithPath);
	ASSERT(logFile != nullptr);
}

void FileOutputPolicy::Process(const LogData& logData)
{
	if(logFile == nullptr)
	{
		return;
	}

	const std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	tm timeInfo;
	Time::GetLocalTime(std::chrono::system_clock::to_time_t(now), &timeInfo);

	StackString<LogData::kMaxMessageLength> strLogLine;
	strLogLine.Sprintf("%02d:%02d:%02d [%s] %s\n", timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec, logData.logType, logData.message);

	logFile->WriteBuffer(strLogLine.Str(), strLogLine.Length());
}
