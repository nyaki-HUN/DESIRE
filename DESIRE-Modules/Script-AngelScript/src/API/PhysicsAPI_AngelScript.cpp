#include "stdafx.h"
#include "API/AngelScriptAPI.h"

#include "Physics/Physics.h"

void RegisterPhysicsAPI_AngelScript(asIScriptEngine *engine)
{
	// Physics
	engine->RegisterObjectType("IPhysics", 0, asOBJ_REF | asOBJ_NOHANDLE);
	engine->RegisterObjectMethod("IPhysics", "bool RaycastAny(const Vector3& p1, const Vector3& p2, int layerMask)", asMETHODPR(Physics, RaycastAny, (const Vector3& p1, const Vector3& p2, int layerMask), bool), asCALL_THISCALL);
	engine->RegisterGlobalProperty("IPhysics Physics", Physics::Get());
}
