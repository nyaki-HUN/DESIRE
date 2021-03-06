#pragma once

#include "Engine/Script/ScriptComponent.h"

class SquirrelScriptComponent : public ScriptComponent
{
public:
	SquirrelScriptComponent(Object& object, HSQUIRRELVM vm);
	~SquirrelScriptComponent() override;

	bool IsValid() const;

	void CallByType(EBuiltinFuncType funcType) override;

	static SQInteger CallFromScript(HSQUIRRELVM vm);

private:
	bool PrepareFunctionCall(const String& functionName) override;
	void ExecuteFunctionCall() override;

	bool AddFunctionCallArg(int arg) override;
	bool AddFunctionCallArg(float arg) override;
	bool AddFunctionCallArg(double arg) override;
	bool AddFunctionCallArg(bool arg) override;
	bool AddFunctionCallArg(void* pArg) override;
	bool AddFunctionCallArg(const String& string) override;

	HSQUIRRELVM m_vm;
	HSQOBJECT m_scriptObject;
	HSQOBJECT m_builtinFunctions[(size_t)EBuiltinFuncType::Num] = {};
	SQInteger m_savedStackTop = 0;
	SQInteger m_numFunctionCallArgs = 0;
};
