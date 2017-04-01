#pragma once

#include "Component/ScriptComponent.h"
#include "Script/Lua/LuaScriptSystem.h"

class LuaScriptComponent : public ScriptComponent
{
public:
	LuaScriptComponent(lua_State *L);
	~LuaScriptComponent();

	void CallByType(EBuiltinFuncType funcType) override;

	int CallFromScript(lua_State *from_L);

private:
	bool PrepareFunctionCall(const char *functionName) override;
	void ExecuteFunctionCall() override;

	bool AddFunctionCallArg(int arg) override;
	bool AddFunctionCallArg(float arg) override;
	bool AddFunctionCallArg(double arg) override;
	bool AddFunctionCallArg(bool arg) override;
	bool AddFunctionCallArg(void *arg) override;
	bool AddFunctionCallArg(const String& string) override;

	lua_State *L;
	int numFunctionCallArgs;
};
