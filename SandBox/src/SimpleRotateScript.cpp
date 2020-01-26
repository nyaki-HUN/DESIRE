#include "SimpleRotateScript.h"

#include "Engine/Application/Application.h"

#include "Engine/Common.h"

#include "Engine/Core/Math/Transform.h"
#include "Engine/Core/Object.h"
#include "Engine/Core/Timer.h"

#include "Engine/Script/ScriptComponent.h"

SimpleRotateScript::SimpleRotateScript()
{
}

SimpleRotateScript::~SimpleRotateScript()
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

	self->GetObject().GetTransform().SetLocalRotation(Quat::CreateRotation(rot, Vector3::AxisX()));
}

std::function<void(std::array<IScript::Arg, 6>)> SimpleRotateScript::GetFunctionToCall(const String& functionName)
{
	DESIRE_UNUSED(functionName);
	return nullptr;
}
