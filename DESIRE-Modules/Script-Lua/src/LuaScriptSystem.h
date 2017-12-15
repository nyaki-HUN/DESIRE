#pragma once

#include "Script/IScriptSystem.h"

struct lua_State;

class LuaScriptSystem : public IScriptSystem
{
public:
	LuaScriptSystem();
	~LuaScriptSystem();

private:
	ScriptComponent* CreateScriptComponent_Internal(const char *scriptName) override;

	static void CompileScript(const char *scriptName, lua_State *L);

	lua_State *L;
};
