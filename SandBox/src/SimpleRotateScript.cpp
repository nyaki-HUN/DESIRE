#include "SimpleRotateScript.h"

#include "Engine/Component/ScriptComponent.h"
#include "Engine/Core/Application.h"
#include "Engine/Core/Modules.h"
#include "Engine/Core/Timer.h"
#include "Engine/Scene/Object.h"
#include "Engine/Scene/Transform.h"

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

std::function<void(std::array<IScript::Arg, 6>)> SimpleRotateScript::GetFunctionToCall(const char *functionName)
{
	DESIRE_UNUSED(functionName);
	return nullptr;
}
