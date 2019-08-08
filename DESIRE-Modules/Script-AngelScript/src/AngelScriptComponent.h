#pragma once

#include "Engine/Script/ScriptComponent.h"
#include "Engine/Core/Container/Array.h"

#include <stdint.h>

class asIScriptContext;
class asIScriptGeneric;
class asIScriptObject;

class AngelScriptComponent : public ScriptComponent
{
public:
	AngelScriptComponent(Object& object);
	~AngelScriptComponent() override;

	void CallByType(EBuiltinFuncType funcType) override;

	static void CallFromScript(asIScriptGeneric* gen);

	asIScriptObject* scriptObject = nullptr;

private:
	bool PrepareFunctionCall(const char* functionName) override;
	void ExecuteFunctionCall() override;

	bool AddFunctionCallArg(int arg) override;
	bool AddFunctionCallArg(float arg) override;
	bool AddFunctionCallArg(double arg) override;
	bool AddFunctionCallArg(bool arg) override;
	bool AddFunctionCallArg(void* arg) override;
	bool AddFunctionCallArg(const String& arg) override;

	asIScriptContext* functionCallCtx = nullptr;
	uint32_t numFunctionCallArgs = 0;
	Array<const void*> functionCallStringArgs;
};
