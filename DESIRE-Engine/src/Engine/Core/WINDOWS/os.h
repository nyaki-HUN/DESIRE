#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

// Helper macro for logging with the system error message for the error code returned by GetLastError()
#define LOG_ERROR_WITH_WIN32_ERRORCODE(MSG, ...)																								\
{																																				\
	char errorMessage[1024];																													\
	const DWORD error = GetLastError();																											\
	FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, error, 0, errorMessage, sizeof(errorMessage), nullptr);	\
	LOG_ERROR("(Error: %lu) - %s" MSG, error, errorMessage, __VA_ARGS__);																		\
}
