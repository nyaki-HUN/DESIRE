#pragma once

#include "Engine/Script/ScriptSystem.h"

class asIScriptEngine;
class asIScriptModule;
class asIScriptContext;
class asIScriptFunction;
class asIScriptGeneric;
struct asSMessageInfo;

class AngelScriptSystem : public ScriptSystem
{
public:
	AngelScriptSystem();
	~AngelScriptSystem() override;

private:
	ScriptComponent* CreateScriptComponentOnObject_Internal(Object& object, const String& scriptName) override;

	static asIScriptModule* CompileScript(const String& scriptName, asIScriptEngine* engine);
	asIScriptContext* CreateScriptContext();

	bool IsBreakpoint(const char* scriptSection, int line, asIScriptFunction* function) const;

	// Callbacks
	static void PrintCallback(asIScriptGeneric* gen);
	static void MessageCallback(const asSMessageInfo* msg, void* thisPtr);
	static asIScriptContext* RequestContextCallback(asIScriptEngine* engine, void* thisPtr);
	static void ReturnContextCallback(asIScriptEngine* engine, asIScriptContext* ctx, void* thisPtr);
	void ExceptionCallback(asIScriptContext* ctx);
	void LineCallback(asIScriptContext* ctx);		// The line callback function is called by the VM for each statement that is executed

	static void* MallocWrapper(size_t size);
	static void FreeWrapper(void* ptr);

	asIScriptEngine* engine = nullptr;
	Array<asIScriptContext*> contextPool;
};
