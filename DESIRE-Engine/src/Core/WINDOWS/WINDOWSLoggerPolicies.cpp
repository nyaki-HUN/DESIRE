#include "stdafx.h"
#include "Core/WINDOWS/WINDOWSLoggerPolicies.h"
#include "Core/WINDOWS/os.h"

WIN32ConsoleOutputPolicy::WIN32ConsoleOutputPolicy()
{
	// We need to create console window on WIN32
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
	FILE *file;
	freopen_s(&file, "CONIN$", "r", stdin);
	freopen_s(&file, "CONOUT$", "w", stdout);
	freopen_s(&file, "CONOUT$", "w", stderr);
}

void WIN32ConsoleOutputPolicy::Process(const Log::LogData& logData)
{
	bool hasToRestoreColor = true;
	switch(logData.logType[0])
	{
		case 'E':	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY); break;							// Error
		case 'W':	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY); break;		// Warning
		case 'M':	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY); break;						// Message
		default:	hasToRestoreColor = false;
	}

	char logLine[Log::MAX_MESSAGE_LENGTH];
	int len = snprintf(logLine, Log::MAX_MESSAGE_LENGTH, "[%s] %s\n", logData.logType, logData.message);
	if(len > 0)
	{
		const size_t numCharsToWrite = std::min((size_t)len, Log::MAX_MESSAGE_LENGTH);
		WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), logLine, (DWORD)numCharsToWrite, nullptr, nullptr);
	}

	if(hasToRestoreColor)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	}
}

void VisualStudioOutputPolicy::Process(const Log::LogData& logData)
{
	// VisualStudio is using the "file(line): message" format for clickable messages
	wchar_t logLine[Log::MAX_MESSAGE_LENGTH];
	swprintf(logLine, Log::MAX_MESSAGE_LENGTH, L"%S(%d): [%S] ", logData.file, logData.line, logData.logType);
	size_t len = wcslen(logLine);
	ASSERT(len + 2 < Log::MAX_MESSAGE_LENGTH);
	len += MultiByteToWideChar(CP_UTF8, 0, logData.message, -1, &logLine[len], (int)(Log::MAX_MESSAGE_LENGTH - (len + 2)));
	logLine[len - 1] = L'\n';
	logLine[len] = L'\0';

	OutputDebugStringW(logLine);
}
