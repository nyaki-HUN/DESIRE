#include "stdafx.h"
#include "Script/Squirrel/API/SquirrelScriptAPI.h"

void RegisterNetworkAPI_Squirrel(Sqrat::RootTable& rootTable)
{
	HSQUIRRELVM vm = rootTable.GetVM();

	// Network
	DESIRE_UNUSED(vm);
//	rootTable.SetInstance("Network", Network::Get());
}
