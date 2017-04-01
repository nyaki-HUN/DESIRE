#include "SimpleRotateScript.h"
#include "Component/ScriptComponent.h"
#include "Component/SceneNodeComponent.h"
#include "Core/Object.h"
#include "Core/Timer.h"
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

	SceneNodeComponent *sceneNode = self->GetObject()->GetComponent<SceneNodeComponent>();
	if(sceneNode != nullptr)
	{
		sceneNode->GetTransform().SetRotation(Quat::CreateRotation(rot, Vector3::AxisX()));
	}

}

std::function<void(std::array<IScript::SArg, 6>)> SimpleRotateScript::GetFunctionToCall(const char *functionName)
{
	DESIRE_UNUSED(functionName);
	return nullptr;
}
