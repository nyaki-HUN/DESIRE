#include "stdafx_Squirrel.h"
#include "SquirrelCallbacks.h"

#include "Engine/Core/Memory/MemorySystem.h"

void* sq_vm_malloc(SQUnsignedInteger size)
{
	return MemorySystem::Alloc(size);
}

void* sq_vm_realloc(void* p, SQUnsignedInteger oldSize, SQUnsignedInteger size)
{
	DESIRE_UNUSED(oldSize);
	return MemorySystem::Realloc(p, size);
}

void sq_vm_free(void* p, SQUnsignedInteger size)
{
	DESIRE_UNUSED(size);
	MemorySystem::Free(p);
}

void SquirrelCallbacks::PrintCallback(HSQUIRRELVM vm, const SQChar* pFormat, ...)
{
	DESIRE_UNUSED(vm);

	LogData logData;
	logData.file = __FILE__;
	logData.line = __LINE__;
	logData.logType = "DBG";

	va_list args;
	va_start(args, pFormat);
	vsnprintf(logData.message, sizeof(logData.message), pFormat, args);
	va_end(args);

	Log::LogWithData(logData);
}

void SquirrelCallbacks::ErrorCallback(HSQUIRRELVM vm, const SQChar* pFormat, ...)
{
	DESIRE_UNUSED(vm);

	LogData logData;
	logData.file = __FILE__;
	logData.line = __LINE__;
	logData.logType = "ERR";

	va_list args;
	va_start(args, pFormat);
	vsnprintf(logData.message, sizeof(logData.message), pFormat, args);
	va_end(args);

	Log::LogWithData(logData);
}

void SquirrelCallbacks::CompilerErrorCallback(HSQUIRRELVM vm, const SQChar* pDescription, const SQChar* pSourceName, SQInteger line, SQInteger column)
{
	DESIRE_UNUSED(vm);

	LOG_ERROR("%s(%d, %d): %s", pSourceName, line, column, pDescription);
}

void SquirrelCallbacks::DebugHookCallback(HSQUIRRELVM vm, SQInteger type, const SQChar* pSourceName, SQInteger line, const SQChar* pFunctionName)
{
	DESIRE_UNUSED(vm);
	DESIRE_UNUSED(type);
	DESIRE_UNUSED(pSourceName);
	DESIRE_UNUSED(line);
	DESIRE_UNUSED(pFunctionName);

	DESIRE_TODO("Squirrel debugging");
}

SQInteger SquirrelCallbacks::RuntimeErrorHandler(HSQUIRRELVM vm)
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
