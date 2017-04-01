#pragma once

#include "Component/ScriptComponent.h"
#include "Script/IScript.h"

class NativeScriptComponent : public ScriptComponent
{
public:
	NativeScriptComponent(std::unique_ptr<IScript> script);
	~NativeScriptComponent();

	void CallByType(EBuiltinFuncType funcType) override;

private:
	bool PrepareFunctionCall(const char *functionName) override;
	void ExecuteFunctionCall() override;

	bool AddFunctionCallArg(int arg) override;
	bool AddFunctionCallArg(float arg) override;
	bool AddFunctionCallArg(double arg) override;
	bool AddFunctionCallArg(bool arg) override;
	bool AddFunctionCallArg(void *arg) override;
	bool AddFunctionCallArg(const String& string) override;

	std::unique_ptr<IScript> script;

	std::function<void(std::array<IScript::SArg, 6>)> functionToCall;
	std::array<IScript::SArg, 6> functionArgs;
	size_t numFunctionCallArgs;
};
