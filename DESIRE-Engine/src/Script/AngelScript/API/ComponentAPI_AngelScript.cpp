#include "stdafx.h"
#include "Script/AngelScript/API/AngelScriptAPI.h"
#include "Script/AngelScript/AngelScriptComponent.h"
#include "Component/PhysicsComponent.h"
#include "Component/RenderComponent.h"
#include "Component/SceneNodeComponent.h"
#include "Scene/Transform.h"

// Template function for registering a class derived from IComponent
#define SCRIPT_API_REGISTER_COMPONENT(CLASS)																												\
	engine->RegisterEnumValue("EComponentTypeID", #CLASS, CLASS::TYPE_ID);																					\
	engine->RegisterObjectType(#CLASS, 0, asOBJ_REF | asOBJ_NOCOUNT);																						\
	engine->RegisterObjectMethod(#CLASS, "Object@ get_object()", asMETHODPR(IComponent, GetObject, () const, Object*), asCALL_THISCALL);					\
	engine->RegisterObjectMethod(#CLASS, "IComponent@ opImplCast()", asFUNCTION((AngelScriptAPI<CLASS>::RefCast<IComponent>)), asCALL_CDECL_OBJLAST);		\
	engine->RegisterObjectMethod("IComponent", #CLASS"@ opImplCast()", asFUNCTION((AngelScriptAPI<IComponent>::RefCast<CLASS>)), asCALL_CDECL_OBJLAST)

void RegisterComponentAPI_AngelScript(asIScriptEngine *engine)
{
	// Transform (we need to register it before SceneNodeComponent)
	engine->RegisterObjectType("Transform", 0, asOBJ_REF | asOBJ_NOHANDLE);
	engine->RegisterObjectMethod("Transform", "const Vector3& get_position() const", asMETHODPR(Transform, GetPosition, () const, const Vector3&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Transform", "void set_position(const Vector3& in)", asMETHODPR(Transform, SetPosition, (const Vector3&), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("Transform", "const Quat& get_rotation() const", asMETHODPR(Transform, GetRotation, () const, const Quat&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Transform", "void set_rotation(const Quat& in)", asMETHODPR(Transform, SetRotation, (const Quat&), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("Transform", "const Vector3& get_scale() const", asMETHODPR(Transform, GetScale, () const, const Vector3&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Transform", "void set_scale(const Vector3& in)", asMETHODPR(Transform, SetScale, (const Vector3&), void), asCALL_THISCALL);

	// IComponent
	engine->RegisterInterfaceMethod("IComponent", "Object@ get_object()");

	// PhysicsComponent
	SCRIPT_API_REGISTER_COMPONENT(PhysicsComponent);
	engine->RegisterObjectMethod("PhysicsComponent", "void set_mass(float)", asMETHODPR(PhysicsComponent, SetMass, (float), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("PhysicsComponent", "float get_mass() const", asMETHODPR(PhysicsComponent, GetMass, () const, float), asCALL_THISCALL);

	// RenderComponent
	SCRIPT_API_REGISTER_COMPONENT(RenderComponent);
	engine->RegisterObjectMethod("RenderComponent", "void set_layer(int)", asMETHODPR(RenderComponent, SetLayer, (int), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("RenderComponent", "int get_layer() const", asMETHODPR(RenderComponent, GetLayer, () const, int), asCALL_THISCALL);

	// SceneNodeComponent
	SCRIPT_API_REGISTER_COMPONENT(SceneNodeComponent);
	engine->RegisterObjectMethod("SceneNodeComponent", "void SetVisible(bool)", asMETHODPR(SceneNodeComponent, SetVisible, (bool), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("SceneNodeComponent", "Transform& get_transform()", asMETHODPR(SceneNodeComponent, GetTransform, (), Transform&), asCALL_THISCALL);
	engine->RegisterObjectMethod("SceneNodeComponent", "SceneNodeComponent@ GetParent()", asMETHODPR(SceneNodeComponent, GetParent, (), SceneNodeComponent*), asCALL_THISCALL);

	// ScriptComponent
	SCRIPT_API_REGISTER_COMPONENT(ScriptComponent);
	engine->RegisterObjectMethod("ScriptComponent", "void Call(const String& in, ?& in)", asFUNCTION(AngelScriptComponent::CallFromScript), asCALL_GENERIC);
	engine->RegisterObjectMethod("ScriptComponent", "void Call(const String& in, ?& in, ?& in)", asFUNCTION(AngelScriptComponent::CallFromScript), asCALL_GENERIC);
	engine->RegisterObjectMethod("ScriptComponent", "void Call(const String& in, ?& in, ?& in, ?& in)", asFUNCTION(AngelScriptComponent::CallFromScript), asCALL_GENERIC);
	engine->RegisterObjectMethod("ScriptComponent", "void Call(const String& in, ?& in, ?& in, ?& in, ?& in)", asFUNCTION(AngelScriptComponent::CallFromScript), asCALL_GENERIC);
	engine->RegisterObjectMethod("ScriptComponent", "void Call(const String& in, ?& in, ?& in, ?& in, ?& in, ?& in)", asFUNCTION(AngelScriptComponent::CallFromScript), asCALL_GENERIC);
	engine->RegisterObjectMethod("ScriptComponent", "void Call(const String& in, ?& in, ?& in, ?& in, ?& in, ?& in, ?& in)", asFUNCTION(AngelScriptComponent::CallFromScript), asCALL_GENERIC);
}
