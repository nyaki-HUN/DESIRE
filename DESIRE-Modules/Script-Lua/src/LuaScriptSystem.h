#pragma once

#include "Engine/Script/ScriptSystem.h"

struct lua_State;

class LuaScriptSystem : public ScriptSystem
{
public:
	LuaScriptSystem();
	~LuaScriptSystem() override;

private:
	ScriptComponent* CreateScriptComponentOnObject_Internal(Object& object, const String& scriptName) override;

	static void CompileScript(const String& scriptName, lua_State* L);

	lua_State* L = nullptr;
};
