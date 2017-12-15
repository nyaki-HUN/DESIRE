#include "stdafx.h"
#include "API/SquirrelScriptAPI.h"

#include "Physics/IPhysics.h"

void RegisterPhysicsAPI_Squirrel(Sqrat::RootTable& rootTable)
{
	HSQUIRRELVM vm = rootTable.GetVM();

	// Physics
	DESIRE_UNUSED(vm);
//	rootTable.SetInstance("Physics", Physics::Get());
}
