#include "stdafx.h"
#include "API/SquirrelScriptAPI.h"

#include "Physics/Physics.h"

void RegisterPhysicsAPI_Squirrel(Sqrat::RootTable& rootTable)
{
	HSQUIRRELVM vm = rootTable.GetVM();

	// Physics
	rootTable.Bind("IPhysics", Sqrat::Class<Physics, Sqrat::NoConstructor<Physics>>(vm, "IPhysics")
		.Func("RaycastAny", &Physics::RaycastAny)
	);
	rootTable.SetInstance("Physics", Physics::Get());
}
