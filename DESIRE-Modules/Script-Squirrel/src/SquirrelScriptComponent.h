#pragma once

#include "Engine/Script/ScriptComponent.h"

#include "squirrel.h"

class SquirrelScriptComponent : public ScriptComponent
{
public:
	SquirrelScriptComponent(Object& object, HSQUIRRELVM vm);
	~SquirrelScriptComponent() override;

	void CallByType(EBuiltinFuncType funcType) override;

	static SQInteger CallFromScript(HSQUIRRELVM vm);

	HSQOBJECT scriptObject;
	HSQOBJECT builtinFunctions[(size_t)EBuiltinFuncType::Num];

private:
	bool PrepareFunctionCall(const char* functionName) override;
	void ExecuteFunctionCall() override;

	bool AddFunctionCallArg(int arg) override;
	bool AddFunctionCallArg(float arg) override;
	bool AddFunctionCallArg(double arg) override;
	bool AddFunctionCallArg(bool arg) override;
	bool AddFunctionCallArg(void* arg) override;
	bool AddFunctionCallArg(const String& string) override;

	HSQUIRRELVM vm = nullptr;
	SQInteger savedStackTop = 0;
	SQInteger numFunctionCallArgs = 0;
};
