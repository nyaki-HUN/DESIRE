#pragma once

#include "Engine/Script/ScriptSystem.h"

class AngelScriptCallbacks;
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

	static asIScriptModule* CompileScript(const String& scriptName, asIScriptEngine& engine);

	bool IsBreakpoint(const char* pScriptSection, int line, asIScriptFunction* pFunction) const;

	asIScriptEngine* m_pEngine = nullptr;
	Array<asIScriptContext*> m_contextPool;

	friend AngelScriptCallbacks;
};
