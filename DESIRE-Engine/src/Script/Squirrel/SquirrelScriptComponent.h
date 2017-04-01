#pragma once

#include "Component/ScriptComponent.h"
#include "Script/Squirrel/SquirrelScriptSystem.h"

class SquirrelScriptComponent : public ScriptComponent
{
public:
	SquirrelScriptComponent(HSQUIRRELVM vm);
	~SquirrelScriptComponent();

	void CallByType(EBuiltinFuncType funcType) override;

	static SQInteger CallFromScript(HSQUIRRELVM vm);

	HSQOBJECT scriptObject;
	HSQOBJECT builtinFunctions[(size_t)EBuiltinFuncType::NUM];

private:
	bool PrepareFunctionCall(const char *functionName) override;
	void ExecuteFunctionCall() override;

	bool AddFunctionCallArg(int arg) override;
	bool AddFunctionCallArg(float arg) override;
	bool AddFunctionCallArg(double arg) override;
	bool AddFunctionCallArg(bool arg) override;
	bool AddFunctionCallArg(void *arg) override;
	bool AddFunctionCallArg(const String& string) override;

	HSQUIRRELVM vm;
	SQInteger savedStackTop;
	SQInteger numFunctionCallArgs;
};
