#include "API/LuaScriptAPI.h"
#include "LuaScriptComponent.h"

#include "Engine/Physics/PhysicsComponent.h"
#include "Engine/Render/RenderComponent.h"
#include "Engine/Scene/Object.h"
#include "Engine/Scene/Transform.h"

void RegisterSceneAPI_Lua(lua_State *L)
{
	// Transform
	luabridge::getGlobalNamespace(L).beginClass<Transform>("Transform")
		.addProperty("localPosition", &Transform::GetLocalPosition, &Transform::SetLocalPosition)
		.addProperty("localRotation", &Transform::GetLocalRotation, &Transform::SetLocalRotation)
		.addProperty("localScale", &Transform::GetLocalScale, &Transform::SetLocalScale)
		.addProperty("position", &Transform::GetPosition, &Transform::SetPosition)
		.addProperty("rotation", &Transform::GetRotation, &Transform::SetRotation)
		.addProperty("scale", &Transform::GetScale, &Transform::SetScale)
		.endClass();

	// Object
	luabridge::getGlobalNamespace(L).beginClass<Object>("Object")
		.addFunction("GetObjectName", &Object::GetObjectName)
		.addFunction("GetID", &Object::GetID)
		.addFunction("SetActive", &Object::SetActive)
		.addFunction("RemoveComponent", &Object::RemoveComponent)
		.addFunction<Component*(Object::*)(int) const>("GetComponent", &Object::GetComponentByTypeID)
		.addFunction<PhysicsComponent*(Object::*)() const>("GetPhysicsComponent", &Object::GetComponent<PhysicsComponent>)
		.addFunction<RenderComponent*(Object::*)() const>("GetRenderComponent", &Object::GetComponent<RenderComponent>)
		.addFunction<LuaScriptComponent*(Object::*)() const>("GetScriptComponent", &Object::GetComponent<LuaScriptComponent>)
		.addProperty("transform", &Object::GetTransform)
		.addFunction("GetParent", &Object::GetParent)
		.endClass();

	// ScriptComponent
	luabridge::getGlobalNamespace(L).deriveClass<LuaScriptComponent, Component>("ScriptComponent")
		.addCFunction("Call", &LuaScriptComponent::CallFromScript)
		.endClass();
}
