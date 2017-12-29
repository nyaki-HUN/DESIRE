#pragma once

#include "AngelScriptSystem.h"

#include "Component/ScriptComponent.h"

class AngelScriptComponent : public ScriptComponent
{
public:
	AngelScriptComponent(Object& object);
	~AngelScriptComponent() override;

	void CallByType(EBuiltinFuncType funcType) override;

	static void CallFromScript(asIScriptGeneric *gen);

	asIScriptObject *scriptObject = nullptr;

private:
	bool PrepareFunctionCall(const char *functionName) override;
	void ExecuteFunctionCall() override;

	bool AddFunctionCallArg(int arg) override;
	bool AddFunctionCallArg(float arg) override;
	bool AddFunctionCallArg(double arg) override;
	bool AddFunctionCallArg(bool arg) override;
	bool AddFunctionCallArg(void *arg) override;
	bool AddFunctionCallArg(const String& arg) override;

	asIScriptContext *functionCallCtx = nullptr;
	uint32_t numFunctionCallArgs = 0;
};
