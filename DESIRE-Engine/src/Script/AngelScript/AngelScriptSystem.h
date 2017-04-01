#pragma once

#include "Script/IScriptSystem.h"
#include "Core/platform.h"

#include <vector>
DESIRE_DISABLE_WARNINGS
#include "Script-AngelScript/include/angelscript.h"
DESIRE_ENABLE_WARNINGS

class String;

class AngelScriptSystem : public IScriptSystem
{
public:
	AngelScriptSystem();
	~AngelScriptSystem();

private:
	ScriptComponent* CreateScriptComponent_Internal(const char *scriptName) override;

	static asIScriptModule* CompileScript(const char *scriptName, asIScriptEngine *engine);
	asIScriptContext* CreateScriptContext();

	bool IsBreakpoint(const char *scriptSection, int line, asIScriptFunction *function) const;

	// Callbacks
	static void PrintCallback(const String& message);
	static void MessageCallback(const asSMessageInfo *msg, void *thisPtr);
	static asIScriptContext* RequestContextCallback(asIScriptEngine *engine, void *thisPtr);
	static void ReturnContextCallback(asIScriptEngine *engine, asIScriptContext *ctx, void *thisPtr);
	void ExceptionCallback(asIScriptContext *ctx);
	void LineCallback(asIScriptContext *ctx);		// The line callback function is called by the VM for each statement that is executed

	asIScriptEngine *engine;
	std::vector<asIScriptContext*> contextPool;
};
