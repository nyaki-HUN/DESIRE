#pragma once

#include "Engine/Script/ScriptComponent.h"
#include "Engine/Core/Container/Array.h"

class asIScriptContext;
class asIScriptFunction;
class asIScriptGeneric;
class asIScriptObject;

class AngelScriptComponent : public ScriptComponent
{
public:
	AngelScriptComponent(Object& object, asIScriptFunction& factoryFunc);
	~AngelScriptComponent();

	bool IsValid() const;

	void CallByType(EBuiltinFuncType funcType) override;

	static void CallFromScript(asIScriptGeneric* pGeneric);

private:
	bool PrepareFunctionCall(const String& functionName) override;
	void ExecuteFunctionCall() override;

	bool AddFunctionCallArg(int arg) override;
	bool AddFunctionCallArg(float arg) override;
	bool AddFunctionCallArg(double arg) override;
	bool AddFunctionCallArg(bool arg) override;
	bool AddFunctionCallArg(void* pArg) override;
	bool AddFunctionCallArg(const String& arg) override;

	asIScriptObject* m_pScriptObject = nullptr;
	asIScriptContext* m_pFunctionCallCtx = nullptr;
	uint32_t m_numFunctionCallArgs = 0;
	Array<const void*> m_functionCallStringArgs;
};
