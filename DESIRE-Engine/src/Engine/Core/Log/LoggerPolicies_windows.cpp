#include "Engine/stdafx.h"

#if DESIRE_PLATFORM_WINDOWS

#include "Engine/Core/Log/LoggerPolicies.h"
#include "Engine/Core/WINDOWS/os.h"

ConsoleWindowOutputPolicy::ConsoleWindowOutputPolicy()
{
	// We need to create console window
	if(!AllocConsole())
	{
		return;
	}

	SetConsoleTitleA("DESIRE Engine Console Log");

	// We want to support UTF-8 characters but if the current font is a raster font, SetConsoleOutputCP does not affect how extended characters are displayed
	CONSOLE_FONT_INFOEX cfInfo;
	cfInfo.cbSize = sizeof(cfInfo);
	cfInfo.nFont = 5;	// The 5th is size 12
	cfInfo.dwFontSize = GetConsoleFontSize(GetStdHandle(STD_OUTPUT_HANDLE), cfInfo.nFont);
	cfInfo.FontFamily = FF_DONTCARE;
	cfInfo.FontWeight = 700;
	wcscpy_s(cfInfo.FaceName, LF_FACESIZE, L"Lucida Console");
	SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfInfo);

	SetConsoleOutputCP(CP_UTF8);

	// Make sure we can always scroll back to the beginning
	COORD coord = { 80, 9999 };
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coord);

	// Redirect stdio functions as well just in case
	FILE* file = nullptr;
	freopen_s(&file, "CONIN$", "r", stdin);
	freopen_s(&file, "CONOUT$", "w", stdout);
	freopen_s(&file, "CONOUT$", "w", stderr);
}

void ConsoleWindowOutputPolicy::Process(const LogData& logData)
{
	bool hasToRestoreColor = true;
	switch(logData.logType[0])
	{
		case 'E':	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY); break;							// Error
		case 'W':	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY); break;		// Warning
		case 'M':	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY); break;						// Message
		default:	hasToRestoreColor = false;
	}

	char logLine[LogData::kMaxMessageLength];
	int len = snprintf(logLine, LogData::kMaxMessageLength, "[%s] %s\n", logData.logType, logData.message);
	if(len > 0)
	{
		const size_t numCharsToWrite = std::min((size_t)len, LogData::kMaxMessageLength);
		WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), logLine, (DWORD)numCharsToWrite, nullptr, nullptr);
	}

	if(hasToRestoreColor)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	}
}

void VisualStudioOutputPolicy::Process(const LogData& logData)
{
	// VisualStudio is using the "file(line): message" format for clickable messages
	wchar_t logLine[LogData::kMaxMessageLength];
	swprintf(logLine, LogData::kMaxMessageLength, L"%S(%d): [%S] ", logData.file, logData.line, logData.logType);
	size_t len = wcslen(logLine);
	ASSERT(len + 2 < LogData::kMaxMessageLength);
	len += MultiByteToWideChar(CP_UTF8, 0, logData.message, -1, &logLine[len], static_cast<int>(LogData::kMaxMessageLength - (len + 2)));
	logLine[len - 1] = L'\n';
	logLine[len] = L'\0';

	OutputDebugStringW(logLine);
}

#endif	// #if DESIRE_PLATFORM_WINDOWS
