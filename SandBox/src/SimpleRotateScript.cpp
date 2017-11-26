#include "SimpleRotateScript.h"
#include "Component/ScriptComponent.h"
#include "Core/Timer.h"
#include "Scene/Object.h"
#include "Scene/Transform.h"

SimpleRotateScript::SimpleRotateScript()
	: rot(0.0f)
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
	rot += Timer::Get()->GetSecDelta();

	self->GetObject()->GetTransform().SetLocalRotation(Quat::CreateRotation(rot, Vector3::AxisX()));
}

std::function<void(std::array<IScript::Arg, 6>)> SimpleRotateScript::GetFunctionToCall(const char *functionName)
{
	DESIRE_UNUSED(functionName);
	return nullptr;
}
