#include "API/SquirrelScriptAPI.h"

void RegisterNetworkAPI_Squirrel(Sqrat::RootTable& rootTable)
{
	HSQUIRRELVM vm = rootTable.GetVM();

	// Network
//	rootTable.SetInstance("Network", Network::Get());
}
