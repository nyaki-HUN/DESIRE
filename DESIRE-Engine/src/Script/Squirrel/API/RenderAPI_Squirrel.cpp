#include "stdafx.h"
#include "Script/Squirrel/API/SquirrelScriptAPI.h"
#include "Render/Render.h"

void RegisterRenderAPI_Squirrel(Sqrat::RootTable& rootTable)
{
	HSQUIRRELVM vm = rootTable.GetVM();

	// Render
	DESIRE_UNUSED(vm);
//	rootTable.SetInstance("Render", Render::Get());
}
