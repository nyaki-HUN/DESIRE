#pragma once

class Callbacks
{
public:
	static void PrintCallback(HSQUIRRELVM vm, const SQChar* format, ...);
	static void ErrorCallback(HSQUIRRELVM vm, const SQChar* format, ...);
	static void CompilerErrorCallback(HSQUIRRELVM vm, const SQChar* desc, const SQChar* source, SQInteger line, SQInteger column);
	static void DebugHookCallback(HSQUIRRELVM vm, SQInteger type, const SQChar* sourcename, SQInteger line, const SQChar* funcname);
	static SQInteger RuntimeErrorHandler(HSQUIRRELVM vm);
};
