#include "API/SquirrelScriptAPI.h"
#include "SquirrelScriptComponent.h"

#include "Engine/Physics/PhysicsComponent.h"
#include "Engine/Render/RenderComponent.h"
#include "Engine/Scene/Object.h"
#include "Engine/Scene/Transform.h"

void RegisterSceneAPI_Squirrel(Sqrat::RootTable& rootTable)
{
	HSQUIRRELVM vm = rootTable.GetVM();

	// Transform
	rootTable.Bind("Transform", Sqrat::Class<Transform, Sqrat::NoConstructor<Transform>>(vm, "Transform")
		.Prop("localPosition", &Transform::GetLocalPosition, &Transform::SetLocalPosition)
		.Prop("localRotation", &Transform::GetLocalRotation, &Transform::SetLocalRotation)
		.Prop("localScale", &Transform::GetLocalScale, &Transform::SetLocalScale)
		.Prop("position", &Transform::GetPosition, &Transform::SetPosition)
		.Prop("rotation", &Transform::GetRotation, &Transform::SetRotation)
		.Prop("scale", &Transform::GetScale, &Transform::SetScale)
	);

	// Object
	rootTable.Bind("Object", Sqrat::Class<Object, Sqrat::NoConstructor<Object>>(vm, "Object")
		.Func("GetObjectName", &Object::GetObjectName)
		.Func("SetActive", &Object::SetActive)
		.Func("RemoveComponent", &Object::RemoveComponent)
		.Func<Component*(Object::*)(int) const>("GetComponent", &Object::GetComponentByTypeId)
		.Func<PhysicsComponent*(Object::*)() const>("GetPhysicsComponent", &Object::GetComponent<PhysicsComponent>)
		.Func<RenderComponent*(Object::*)() const>("GetRenderComponent", &Object::GetComponent<RenderComponent>)
		.Func<SquirrelScriptComponent*(Object::*)() const>("GetScriptComponent", &Object::GetComponent<SquirrelScriptComponent>)
		.Prop("transform", &Object::GetTransform)
		.Func("GetParent", &Object::GetParent)
	);
}
