#pragma once

#include "Engine/Script/ScriptSystem.h"

#include "../Externals/Squirrel/include/squirrel.h"

class SquirrelScriptSystem : public ScriptSystem
{
public:
	SquirrelScriptSystem();
	~SquirrelScriptSystem() override;

private:
	ScriptComponent* CreateScriptComponentOnObject_Internal(Object& object, const char* scriptName) override;

	static void CompileScript(const char* scriptName, HSQUIRRELVM vm);

	// Callbacks
	static void PrintCallback(HSQUIRRELVM vm, const SQChar* format, ...);
	static void ErrorCallback(HSQUIRRELVM vm, const SQChar* format, ...);
	static void CompilerErrorCallback(HSQUIRRELVM vm, const SQChar* desc, const SQChar* source, SQInteger line, SQInteger column);
	static void DebugHookCallback(HSQUIRRELVM vm, SQInteger type, const SQChar* sourcename, SQInteger line, const SQChar* funcname);
	static SQInteger RuntimeErrorHandler(HSQUIRRELVM vm);

	HSQUIRRELVM vm = nullptr;
};
