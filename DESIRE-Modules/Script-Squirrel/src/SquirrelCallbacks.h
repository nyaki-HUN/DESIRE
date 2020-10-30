#pragma once

class SquirrelCallbacks
{
public:
	static void PrintCallback(HSQUIRRELVM vm, const SQChar* pFormat, ...);
	static void ErrorCallback(HSQUIRRELVM vm, const SQChar* pFormat, ...);
	static void CompilerErrorCallback(HSQUIRRELVM vm, const SQChar* pDescription, const SQChar* pSource, SQInteger line, SQInteger column);
	static void DebugHookCallback(HSQUIRRELVM vm, SQInteger type, const SQChar* pSourceName, SQInteger line, const SQChar* pFunctionName);
	static SQInteger RuntimeErrorHandler(HSQUIRRELVM vm);
};
