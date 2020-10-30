#pragma once

#include "Engine/Script/ScriptSystem.h"

typedef struct SQVM* HSQUIRRELVM;

class SquirrelScriptSystem : public ScriptSystem
{
public:
	SquirrelScriptSystem();
	~SquirrelScriptSystem() override;

private:
	ScriptComponent* CreateScriptComponentOnObject_Internal(Object& object, const String& scriptName) override;

	static void CompileScript(const String& scriptName, HSQUIRRELVM vm);

	HSQUIRRELVM m_vm = nullptr;
};
