#include "stdafx.h"
#include "Script/Squirrel/API/SquirrelScriptAPI.h"
#include "Script/Squirrel/SquirrelScriptComponent.h"
#include "Component/PhysicsComponent.h"
#include "Component/RenderComponent.h"
#include "Component/SceneNodeComponent.h"
#include "Core/Object.h"
#include "Scene/Transform.h"

void RegisterComponentAPI_Squirrel(Sqrat::RootTable& rootTable)
{
	HSQUIRRELVM vm = rootTable.GetVM();

	// Object
	rootTable.Bind("Object", Sqrat::Class<Object, Sqrat::NoConstructor<Object>>(vm, "Object")
		.Func("GetObjectName", &Object::GetObjectName)
		.Func("GetID", &Object::GetID)
		.Func<IComponent*(Object::*)(int)>("GetComponent", &Object::GetComponentByTypeID)
		.Func<PhysicsComponent*(Object::*)()>("GetPhysicsComponent", &Object::GetComponent<PhysicsComponent>)
		.Func<RenderComponent*(Object::*)()>("GetRenderComponent", &Object::GetComponent<RenderComponent>)
		.Func<SceneNodeComponent*(Object::*)()>("GetSceneNodeComponent", &Object::GetComponent<SceneNodeComponent>)
		.Func<SquirrelScriptComponent*(Object::*)()>("GetScriptComponent", &Object::GetComponent<SquirrelScriptComponent>)
	);

	// Transform
	rootTable.Bind("Transform", Sqrat::Class<Transform, Sqrat::NoConstructor<Transform>>(vm, "Transform")
		.Prop("position", &Transform::GetPosition, &Transform::SetPosition)
		.Prop("rotation", &Transform::GetRotation, &Transform::SetRotation)
		.Prop("scale", &Transform::GetScale, &Transform::SetScale)
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
		.Prop("alpha", &RenderComponent::GetInstanceAlpha, &RenderComponent::SetInstanceAlpha)
	);

	// SceneNodeComponent
	rootTable.Bind("SceneNodeComponent", Sqrat::DerivedClass<SceneNodeComponent, IComponent, Sqrat::NoConstructor<SceneNodeComponent>>(vm, "SceneNodeComponent")
		.Func("SetVisible", &SceneNodeComponent::SetVisible)
		.Prop("transform", &SceneNodeComponent::GetTransform)
		.Func("GetParent", &SceneNodeComponent::GetParent)
	);

	// ScriptComponent
	rootTable.Bind("ScriptComponent", Sqrat::DerivedClass<SquirrelScriptComponent, IComponent, Sqrat::NoConstructor<SquirrelScriptComponent>>(vm, "ScriptComponent")
		.SquirrelFunc("Call", &SquirrelScriptComponent::CallFromScript)
	);
}
