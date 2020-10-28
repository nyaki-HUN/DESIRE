#pragma once

#include "Engine/Script/IScript.h"

class SimpleRotateScript : public IScript
{
public:
	SimpleRotateScript(ScriptComponent& scriptComponent);

	void Init() override;
	void Kill() override;
	void Update() override;

	std::function<void(std::array<IScript::Arg, 6>)> GetFunctionToCall(const String& functionName) override;

private:
	float rot = 0.0f;
};
