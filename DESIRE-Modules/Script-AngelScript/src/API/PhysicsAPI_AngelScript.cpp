#include "stdafx.h"
#include "Script/AngelScript/API/AngelScriptAPI.h"
#include "Physics/IPhysics.h"

void RegisterPhysicsAPI_AngelScript(asIScriptEngine *engine)
{
	// Physics
	engine->RegisterObjectType("IPhysics", 0, asOBJ_REF | asOBJ_NOHANDLE);
	engine->RegisterGlobalProperty("IPhysics Physics", IPhysics::Get());
}
