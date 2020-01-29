#include "stdafx_Squirrel.h"
#include "Callbacks.h"

void Callbacks::PrintCallback(HSQUIRRELVM vm, const SQChar* format, ...)
{
	DESIRE_UNUSED(vm);

	LogData logData;
	logData.file = __FILE__;
	logData.line = __LINE__;
	logData.logType = "DBG";

	va_list args;
	va_start(args, format);
	vsnprintf(logData.message, sizeof(logData.message), format, args);
	va_end(args);

	Log::LogWithData(logData);
}

void Callbacks::ErrorCallback(HSQUIRRELVM vm, const SQChar* format, ...)
{
	DESIRE_UNUSED(vm);

	LogData logData;
	logData.file = __FILE__;
	logData.line = __LINE__;
	logData.logType = "ERR";

	va_list args;
	va_start(args, format);
	vsnprintf(logData.message, sizeof(logData.message), format, args);
	va_end(args);

	Log::LogWithData(logData);
}

void Callbacks::CompilerErrorCallback(HSQUIRRELVM vm, const SQChar* desc, const SQChar* source, SQInteger line, SQInteger column)
{
	DESIRE_UNUSED(vm);

	LOG_ERROR("%s(%d, %d): %s", source, line, column, desc);
}

void Callbacks::DebugHookCallback(HSQUIRRELVM vm, SQInteger type, const SQChar* sourcename, SQInteger line, const SQChar* funcname)
{
	DESIRE_UNUSED(vm);
	DESIRE_UNUSED(type);
	DESIRE_UNUSED(sourcename);
	DESIRE_UNUSED(line);
	DESIRE_UNUSED(funcname);

	DESIRE_TODO("Squirrel debugging");
}

SQInteger Callbacks::RuntimeErrorHandler(HSQUIRRELVM vm)
{
	const SQChar* errorStr = 0;
	if(sq_gettop(vm) >= 1)
	{
		if(SQ_SUCCEEDED(sq_getstring(vm, 2, &errorStr)))
		{
			LOG_ERROR("Runtime error: %s", errorStr);
		}
		else
		{
			LOG_ERROR("Unknown runtime error");
		}

		// TODO: print callstack, check sqstd_printcallstack() function
	}

	return 0;
}
