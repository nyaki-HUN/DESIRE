#pragma once

#include "Engine/Script/IScript.h"
#include "Engine/Script/ScriptComponent.h"
#include "Engine/Script/ScriptSystem.h"

class NativeScriptComponent : public ScriptComponent
{
public:
	NativeScriptComponent(Object& object, ScriptSystem::ScriptFactory::Func_t scriptFactoryFunc);
	~NativeScriptComponent() override;

	void CallByType(EBuiltinFuncType funcType) override;

private:
	bool PrepareFunctionCall(const String& functionName) override;
	void ExecuteFunctionCall() override;

	bool AddFunctionCallArg(int arg) override;
	bool AddFunctionCallArg(float arg) override;
	bool AddFunctionCallArg(double arg) override;
	bool AddFunctionCallArg(bool arg) override;
	bool AddFunctionCallArg(void* pArg) override;
	bool AddFunctionCallArg(const String& string) override;

	std::unique_ptr<IScript> m_spScript;

	std::function<void(std::array<IScript::Arg, 6>)> m_functionToCall = nullptr;
	std::array<IScript::Arg, 6> m_functionArgs;
	size_t m_numFunctionCallArgs = 0;
};
