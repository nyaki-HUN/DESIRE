#pragma once

#include "Engine/Script/ScriptComponent.h"

class LuaScriptComponent : public ScriptComponent
{
public:
	LuaScriptComponent(Object& object, lua_State* L);

	void CallByType(EBuiltinFuncType funcType) override;

	int CallFromScript(sol::this_state ts);

private:
	bool PrepareFunctionCall(const String& functionName) override;
	void ExecuteFunctionCall() override;

	bool AddFunctionCallArg(int arg) override;
	bool AddFunctionCallArg(float arg) override;
	bool AddFunctionCallArg(double arg) override;
	bool AddFunctionCallArg(bool arg) override;
	bool AddFunctionCallArg(void* pArg) override;
	bool AddFunctionCallArg(const String& string) override;

	lua_State* m_L = nullptr;
	int m_numFunctionCallArgs = 0;
};
