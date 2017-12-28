#include "stdafx.h"
#include "API/SquirrelScriptAPI.h"
#include "SquirrelScriptComponent.h"

#include "Component/PhysicsComponent.h"
#include "Component/RenderComponent.h"
#include "Scene/Object.h"
#include "Scene/Transform.h"

void RegisterComponentAPI_Squirrel(Sqrat::RootTable& rootTable)
{
	HSQUIRRELVM vm = rootTable.GetVM();

	// Transform
	rootTable.Bind("Transform", Sqrat::Class<Transform, Sqrat::NoConstructor<Transform>>(vm, "Transform")
		.Prop("localPosition", &Transform::GetLocalPosition, &Transform::SetLocalPosition)
		.Prop("localRotation", &Transform::GetLocalRotation, &Transform::SetLocalRotation)
		.Prop("localScale", &Transform::GetLocalScale, &Transform::SetLocalScale)
		.Prop("position", &Transform::GetPosition, &Transform::SetPosition)
		.Prop("rotation", &Transform::GetPosition, &Transform::SetPosition)
		.Prop("scale", &Transform::GetScale, &Transform::SetScale)
	);

	// Object
	rootTable.Bind("Object", Sqrat::Class<Object, Sqrat::NoConstructor<Object>>(vm, "Object")
		.Func("GetObjectName", &Object::GetObjectName)
		.Func("GetID", &Object::GetID)
		.Func("SetActive", &Object::SetActive)
		.Func<IComponent*(Object::*)(int)>("GetComponent", &Object::GetComponentByTypeID)
		.Func<PhysicsComponent*(Object::*)()>("GetPhysicsComponent", &Object::GetComponent<PhysicsComponent>)
		.Func<RenderComponent*(Object::*)()>("GetRenderComponent", &Object::GetComponent<RenderComponent>)
		.Func<SquirrelScriptComponent*(Object::*)()>("GetScriptComponent", &Object::GetComponent<SquirrelScriptComponent>)
		.Prop("transform", &Object::GetTransform)
		.Func("GetParent", &Object::GetParent)
	);

	// IComponent
	rootTable.Bind("IComponent", Sqrat::Class<IComponent, Sqrat::NoConstructor<IComponent>>(vm, "IComponent")
		.Prop("object", &IComponent::GetObject)
	);

	// PhysicsComponent
	rootTable.Bind("PhysicsComponent", Sqrat::DerivedClass<PhysicsComponent, IComponent, Sqrat::NoConstructor<PhysicsComponent>>(vm, "PhysicsComponent")
		.Prop("mass", &PhysicsComponent::GetMass, &PhysicsComponent::SetMass)
	);

	// RenderComponent
	rootTable.Bind("RenderComponent", Sqrat::DerivedClass<RenderComponent, IComponent, Sqrat::NoConstructor<RenderComponent>>(vm, "RenderComponent")
		.Prop("layer", &RenderComponent::GetLayer, &RenderComponent::SetLayer)
	);

	// ScriptComponent
	rootTable.Bind("ScriptComponent", Sqrat::DerivedClass<SquirrelScriptComponent, IComponent, Sqrat::NoConstructor<SquirrelScriptComponent>>(vm, "ScriptComponent")
		.SquirrelFunc("Call", &SquirrelScriptComponent::CallFromScript)
	);
}
