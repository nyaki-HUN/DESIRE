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
	static int LuaPanicFunc(lua_State* L);

	static void* ReallocWrapper(void* userData, void* ptr, size_t oldSize, size_t newSize);

	lua_State* L = nullptr;
};
