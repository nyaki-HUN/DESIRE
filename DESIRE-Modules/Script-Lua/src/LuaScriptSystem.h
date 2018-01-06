#pragma once

#include "Script/IScriptSystem.h"

struct lua_State;

class LuaScriptSystem : public IScriptSystem
{
public:
	LuaScriptSystem();
	~LuaScriptSystem() override;

private:
	void CreateScriptComponentOnObject_Internal(Object& object, const char *scriptName) override;

	static void CompileScript(const char *scriptName, lua_State *L);

	lua_State *L = nullptr;
};
