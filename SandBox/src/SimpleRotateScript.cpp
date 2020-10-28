#include "stdafx.h"
#include "SimpleRotateScript.h"

#include "Engine/Application/Application.h"

#include "Engine/Core/Math/Transform.h"
#include "Engine/Core/Object.h"
#include "Engine/Core/Timer.h"

#include "Engine/Script/ScriptComponent.h"

SimpleRotateScript::SimpleRotateScript(ScriptComponent& scriptComponent)
	: IScript(scriptComponent)
{
}

void SimpleRotateScript::Init()
{
}

void SimpleRotateScript::Kill()
{
}

void SimpleRotateScript::Update()
{
	rot += Modules::Application->GetTimer()->GetSecDelta();

	self.GetObject().GetTransform().SetLocalRotation(Quat::CreateRotation(rot, Vector3::AxisX()));
}

std::function<void(std::array<IScript::Arg, 6>)> SimpleRotateScript::GetFunctionToCall(const String& functionName)
{
	DESIRE_UNUSED(functionName);
	return nullptr;
}
