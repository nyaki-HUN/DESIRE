#include "API/AngelScriptAPI.h"

#include "Engine/Scene/Object.h"
#include "Engine/Scene/Transform.h"

static Vector3* Transform_GetPosition(const Transform& transform)	{ return new Vector3(transform.GetPosition()); }
static Quat* Transform_GetRotation(const Transform& transform)		{ return new Quat(transform.GetRotation()); }
static Vector3* Transform_GetScale(const Transform& transform)		{ return new Vector3(transform.GetScale()); }

void RegisterSceneAPI_AngelScript(asIScriptEngine *engine)
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
	engine->RegisterObjectMethod("Transform", "Quat& get_rotation() const", asFUNCTION(Transform_GetRotation), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Transform", "void set_rotation(const Quat& in)", asMETHODPR(Transform, SetRotation, (const Quat&), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("Transform", "Vector3& get_scale() const", asFUNCTION(Transform_GetScale), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Transform", "void set_scale(const Vector3& in)", asMETHODPR(Transform, SetScale, (const Vector3&), void), asCALL_THISCALL);

	// Object
	// Note: ObjectType is already registered in RegisterCoreAPI_AngelScript()
	engine->RegisterObjectMethod("Object", "string GetObjectName() const", asFUNCTION((AngelScriptGenericAPI<Object>::MakeStringRvFromMemberFunc<&Object::GetObjectName>)), asCALL_GENERIC);
	engine->RegisterObjectMethod("Object", "void SetActive(bool)", asMETHODPR(Object, SetActive, (bool), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("Object", "void RemoveComponent(Component@)", asMETHODPR(Object, RemoveComponent, (const Component*), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("Object", "Component@ GetComponent(EComponent) const", asMETHODPR(Object, GetComponentByTypeId, (int) const, Component*), asCALL_THISCALL);
	engine->RegisterObjectMethod("Object", "Transform& get_transform() const", asMETHODPR(Object, GetTransform, () const, Transform&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Object", "Object@ GetParent() const", asMETHODPR(Object, GetParent, () const, Object*), asCALL_THISCALL);
}
