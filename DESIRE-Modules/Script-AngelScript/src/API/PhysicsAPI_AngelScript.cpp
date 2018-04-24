#include "API/AngelScriptAPI.h"

#include "Engine/Core/Modules.h"
#include "Engine/Physics/Physics.h"

void RegisterPhysicsAPI_AngelScript(asIScriptEngine *engine)
{
	Physics *physics = Modules::Physics.get();
	if(physics == nullptr)
	{
		return;
	}

	int result = asSUCCESS;

	// Physics
	result = engine->RegisterObjectType("IPhysics", 0, asOBJ_REF | asOBJ_NOHANDLE);																																		ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("IPhysics", "bool RaycastAny(const Vector3& in, const Vector3& in, int)", asMETHODPR(Physics, RaycastAny, (const Vector3&, const Vector3&, int), bool), asCALL_THISCALL);		ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalProperty("IPhysics Physics", physics);																																				ASSERT(result >= asSUCCESS);
}
