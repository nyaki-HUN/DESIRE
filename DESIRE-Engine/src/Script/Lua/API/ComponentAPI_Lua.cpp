#include "stdafx.h"
#include "Script/Lua/API/LuaScriptAPI.h"
#include "Script/Lua/LuaScriptComponent.h"
#include "Component/PhysicsComponent.h"
#include "Component/RenderComponent.h"
#include "Component/SceneNodeComponent.h"
#include "Core/Object.h"
#include "Scene/Transform.h"

void RegisterComponentAPI_Lua(lua_State *L)
{
	// Object
	luabridge::getGlobalNamespace(L).beginClass<Object>("Object")
		.addFunction("GetObjectName", &Object::GetObjectName)
		.addFunction("GetID", &Object::GetID)
		.addFunction<IComponent*(Object::*)(int)>("GetComponent", &Object::GetComponentByTypeID)
		.addFunction<PhysicsComponent*(Object::*)()>("GetPhysicsComponent", &Object::GetComponent<PhysicsComponent>)
		.addFunction<RenderComponent*(Object::*)()>("GetRenderComponent", &Object::GetComponent<RenderComponent>)
		.addFunction<SceneNodeComponent*(Object::*)()>("GetSceneNodeComponent", &Object::GetComponent<SceneNodeComponent>)
		.addFunction<LuaScriptComponent*(Object::*)()>("GetScriptComponent", &Object::GetComponent<LuaScriptComponent>)
		.endClass();

	// Transform
	luabridge::getGlobalNamespace(L).beginClass<Transform>("Transform")
		.addProperty("position", &Transform::GetPosition, &Transform::SetPosition)
		.addProperty("rotation", &Transform::GetRotation, &Transform::SetRotation)
		.addProperty("scale", &Transform::GetScale, &Transform::SetScale)
		.endClass();

	// IComponent
	luabridge::getGlobalNamespace(L).beginClass<IComponent>("IComponent")
		.addProperty("object", &IComponent::GetObject)
		.endClass();

	// PhysicsComponent
	luabridge::getGlobalNamespace(L).deriveClass<PhysicsComponent, IComponent>("PhysicsComponent")
		.addProperty("mass", &PhysicsComponent::GetMass, &PhysicsComponent::SetMass)
		.endClass();

	// RenderComponent
	luabridge::getGlobalNamespace(L).deriveClass<RenderComponent, IComponent>("RenderComponent")
		.addProperty("layer", &RenderComponent::GetLayer, &RenderComponent::SetLayer)
		.endClass();

	// SceneNodeComponent
	luabridge::getGlobalNamespace(L).deriveClass<SceneNodeComponent, IComponent>("SceneNodeComponent")
		.addFunction("SetVisible", &SceneNodeComponent::SetVisible)
		.addProperty("transform", &SceneNodeComponent::GetTransform)
		.addFunction("GetParent", &SceneNodeComponent::GetParent)
		.endClass();

	// ScriptComponent
	luabridge::getGlobalNamespace(L).deriveClass<LuaScriptComponent, IComponent>("ScriptComponent")
		.addCFunction("Call", &LuaScriptComponent::CallFromScript)
		.endClass();
}
