#include "Engine/stdafx.h"
#include "Engine/Core/Log/LoggerPolicies.h"

#if DESIRE_PLATFORM_WINDOWS

#include "Engine/Core/String/StackString.h"
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
	CONSOLE_FONT_INFOEX fontInfo;
	fontInfo.cbSize = sizeof(fontInfo);
	fontInfo.nFont = 5;	// The 5th is size 12
	fontInfo.dwFontSize = GetConsoleFontSize(GetStdHandle(STD_OUTPUT_HANDLE), fontInfo.nFont);
	fontInfo.FontFamily = FF_DONTCARE;
	fontInfo.FontWeight = 700;
	wcscpy_s(fontInfo.FaceName, LF_FACESIZE, L"Lucida Console");
	SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &fontInfo);

	SetConsoleOutputCP(CP_UTF8);

	// Make sure we can always scroll back to the beginning
	const COORD coord = { 80, 9999 };
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coord);

	// Redirect stdio functions as well just in case
	FILE* pFile = nullptr;
	freopen_s(&pFile, "CONIN$", "r", stdin);
	freopen_s(&pFile, "CONOUT$", "w", stdout);
	freopen_s(&pFile, "CONOUT$", "w", stderr);
}

void ConsoleWindowOutputPolicy::Process(const LogData& logData)
{
	bool hasToRestoreColor = true;
	switch(logData.pLogType[0])
	{
		case 'E':	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY); break;							// Error
		case 'W':	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY); break;		// Warning
		case 'M':	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY); break;						// Message
		default:	hasToRestoreColor = false;
	}

	StackString<LogData::kMaxMessageLength> logLine;
	logLine.Sprintf("[%s] %s\n", logData.pLogType, logData.message);
	WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), logLine.Str(), static_cast<DWORD>(logLine.Length()), nullptr, nullptr);

	if(hasToRestoreColor)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	}
}

void VisualStudioOutputPolicy::Process(const LogData& logData)
{
	// VisualStudio is using the "file(line): message" format for clickable messages
	StackString<LogData::kMaxMessageLength> logLine;
	logLine.Sprintf("%s(%d): [%s] %s\n", logData.pFilename, logData.line, logData.pLogType, logData.message);

	wchar_t logLineW[LogData::kMaxMessageLength] = {};
	MultiByteToWideChar(CP_UTF8, 0, logLine.Str(), -1, logLineW, static_cast<int>(LogData::kMaxMessageLength));

	OutputDebugStringW(logLineW);
}

#endif	// #if DESIRE_PLATFORM_WINDOWS
