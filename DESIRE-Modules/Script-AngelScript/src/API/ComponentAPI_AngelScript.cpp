#include "stdafx.h"
#include "API/AngelScriptAPI.h"
#include "AngelScriptComponent.h"

#include "Component/PhysicsComponent.h"
#include "Component/RenderComponent.h"
#include "Scene/Object.h"
#include "Scene/Transform.h"

// Template function for registering a class derived from IComponent
#define SCRIPT_API_REGISTER_COMPONENT(CLASS)																												\
	engine->RegisterEnumValue("EComponentTypeID", #CLASS, CLASS::TYPE_ID);																					\
	engine->RegisterObjectType(#CLASS, 0, asOBJ_REF | asOBJ_NOCOUNT);																						\
	engine->RegisterObjectMethod(#CLASS, "Object@ get_object()", asMETHODPR(IComponent, GetObject, () const, Object*), asCALL_THISCALL);					\
	engine->RegisterObjectMethod(#CLASS, "IComponent@ opImplCast()", asFUNCTION((AngelScriptAPI<CLASS>::RefCast<IComponent>)), asCALL_CDECL_OBJLAST);		\
	engine->RegisterObjectMethod("IComponent", #CLASS"@ opImplCast()", asFUNCTION((AngelScriptAPI<IComponent>::RefCast<CLASS>)), asCALL_CDECL_OBJLAST)

static Vector3* Transform_GetPosition(const Transform& transform)
{
	return new Vector3(transform.GetPosition());
}

void RegisterComponentAPI_AngelScript(asIScriptEngine *engine)
{
	// Transform
	engine->RegisterObjectType("Transform", 0, asOBJ_REF | asOBJ_NOHANDLE);
	engine->RegisterObjectMethod("Transform", "const Vector3& get_localPosition() const", asMETHODPR(Transform, GetLocalPosition, () const, const Vector3&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Transform", "void set_localPosition(const Vector3& in)", asMETHODPR(Transform, SetLocalPosition, (const Vector3&), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("Transform", "const Quat& get_localRotation() const", asMETHODPR(Transform, GetLocalRotation, () const, const Quat&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Transform", "void set_localRotation(const Quat& in)", asMETHODPR(Transform, SetLocalRotation, (const Quat&), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("Transform", "const Vector3& get_localScale() const", asMETHODPR(Transform, GetLocalScale, () const, const Vector3&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Transform", "void set_localScale(const Vector3& in)", asMETHODPR(Transform, SetLocalScale, (const Vector3&), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("Transform", "Vector3@ get_position()", asFUNCTION(Transform_GetPosition), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Transform", "void set_position(const Vector3& in)", asMETHODPR(Transform, SetPosition, (const Vector3&), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("Transform", "void set_rotation(const Quat& in)", asMETHODPR(Transform, SetRotation, (const Quat&), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("Transform", "void set_scale(const Vector3& in)", asMETHODPR(Transform, SetScale, (const Vector3&), void), asCALL_THISCALL);

	// Object
	engine->RegisterObjectType("Object", 0, asOBJ_REF | asOBJ_NOCOUNT);
	engine->RegisterObjectMethod("Object", "String GetObjectName() const", asFUNCTION((AngelScriptGenericAPI<Object>::MakeStringRvFromMemberFunc<&Object::GetObjectName>)), asCALL_GENERIC);
	engine->RegisterObjectMethod("Object", "uint32 GetID() const", asMETHODPR(Object, GetID, () const, uint32_t), asCALL_THISCALL);
	engine->RegisterObjectMethod("Object", "void SetActive(bool)", asMETHODPR(Object, SetActive, (bool), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("Object", "IComponent@ GetComponent(EComponentTypeID typeID)", asMETHODPR(Object, GetComponentByTypeID, (int), IComponent*), asCALL_THISCALL);
	engine->RegisterObjectMethod("Object", "Transform& get_transform() const", asMETHODPR(Object, GetTransform, () const, Transform&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Object", "Object@ GetParent() const", asMETHODPR(Object, GetParent, () const, Object*), asCALL_THISCALL);

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

	// ScriptComponent
	SCRIPT_API_REGISTER_COMPONENT(ScriptComponent);
	engine->RegisterObjectMethod("ScriptComponent", "void Call(const String& in, ?& in)", asFUNCTION(AngelScriptComponent::CallFromScript), asCALL_GENERIC);
	engine->RegisterObjectMethod("ScriptComponent", "void Call(const String& in, ?& in, ?& in)", asFUNCTION(AngelScriptComponent::CallFromScript), asCALL_GENERIC);
	engine->RegisterObjectMethod("ScriptComponent", "void Call(const String& in, ?& in, ?& in, ?& in)", asFUNCTION(AngelScriptComponent::CallFromScript), asCALL_GENERIC);
	engine->RegisterObjectMethod("ScriptComponent", "void Call(const String& in, ?& in, ?& in, ?& in, ?& in)", asFUNCTION(AngelScriptComponent::CallFromScript), asCALL_GENERIC);
	engine->RegisterObjectMethod("ScriptComponent", "void Call(const String& in, ?& in, ?& in, ?& in, ?& in, ?& in)", asFUNCTION(AngelScriptComponent::CallFromScript), asCALL_GENERIC);
	engine->RegisterObjectMethod("ScriptComponent", "void Call(const String& in, ?& in, ?& in, ?& in, ?& in, ?& in, ?& in)", asFUNCTION(AngelScriptComponent::CallFromScript), asCALL_GENERIC);
}
