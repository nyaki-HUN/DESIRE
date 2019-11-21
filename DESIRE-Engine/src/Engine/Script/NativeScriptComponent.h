#pragma once

#include "Engine/Script/ScriptComponent.h"
#include "Engine/Script/IScript.h"

class NativeScriptComponent : public ScriptComponent
{
public:
	NativeScriptComponent(Object& object, std::unique_ptr<IScript>&& script);
	~NativeScriptComponent() override;

	void CallByType(EBuiltinFuncType funcType) override;

private:
	bool PrepareFunctionCall(const String& functionName) override;
	void ExecuteFunctionCall() override;

	bool AddFunctionCallArg(int arg) override;
	bool AddFunctionCallArg(float arg) override;
	bool AddFunctionCallArg(double arg) override;
	bool AddFunctionCallArg(bool arg) override;
	bool AddFunctionCallArg(void *arg) override;
	bool AddFunctionCallArg(const String& string) override;

	std::unique_ptr<IScript> script;

	std::function<void(std::array<IScript::Arg, 6>)> functionToCall = nullptr;
	std::array<IScript::Arg, 6> functionArgs;
	size_t numFunctionCallArgs = 0;
};
