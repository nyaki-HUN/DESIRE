#include "stdafx_AngelScript.h"
#include "API/AngelScriptAPI.h"

#include "Engine/Physics/Physics.h"
#include "Engine/Physics/PhysicsComponent.h"

void RegisterPhysicsAPI_AngelScript(asIScriptEngine& engine)
{
	if(Modules::Physics == nullptr)
	{
		return;
	}

	int32_t result = asSUCCESS;

	// PhysicsComponent
	ANGELSCRIPT_API_REGISTER_COMPONENT(PhysicsComponent);
	result = engine.RegisterObjectMethod("PhysicsComponent", "void SetMass(float)", asMETHOD(PhysicsComponent, SetMass), asCALL_THISCALL);		ASSERT(result >= asSUCCESS);
	result = engine.RegisterObjectMethod("PhysicsComponent", "float GetMass() const", asMETHOD(PhysicsComponent, GetMass), asCALL_THISCALL);	ASSERT(result >= asSUCCESS);

	// Physics
	result = engine.RegisterObjectType("IPhysics", 0, asOBJ_REF | asOBJ_NOHANDLE);																					ASSERT(result >= asSUCCESS);
	result = engine.RegisterObjectMethod("IPhysics", "bool RaycastAny(const Vector3& in, const Vector3& in, int)", asMETHOD(Physics, RaycastAny), asCALL_THISCALL);	ASSERT(result >= asSUCCESS);
	result = engine.RegisterGlobalProperty("IPhysics Physics", Modules::Physics.get());																				ASSERT(result >= asSUCCESS);
}
