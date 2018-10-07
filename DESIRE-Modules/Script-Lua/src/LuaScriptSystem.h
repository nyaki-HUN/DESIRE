#pragma once

#include "Engine/Script/ScriptSystem.h"

struct lua_State;

class LuaScriptSystem : public ScriptSystem
{
public:
	LuaScriptSystem();
	~LuaScriptSystem() override;

private:
	ScriptComponent* CreateScriptComponentOnObject_Internal(Object& object, const char *scriptName) override;

	static void CompileScript(const char *scriptName, lua_State *L);

	lua_State *L = nullptr;
};
