#pragma once

#include "Engine/Script/ScriptSystem.h"

class asIScriptEngine;
class asIScriptModule;
class asIScriptContext;
class asIScriptFunction;

class AngelScriptSystem : public ScriptSystem
{
public:
	AngelScriptSystem();
	~AngelScriptSystem() override;

private:
	ScriptComponent* CreateScriptComponentOnObject_Internal(Object& object, const String& scriptName) override;

	static asIScriptModule* CompileScript(const String& scriptName, asIScriptEngine* engine);

	bool IsBreakpoint(const char* scriptSection, int line, asIScriptFunction* function) const;

	asIScriptEngine* engine = nullptr;
	Array<asIScriptContext*> contextPool;

	friend class AngelScriptCallbacks;
};
