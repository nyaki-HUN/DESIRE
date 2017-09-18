#include "stdafx.h"
#include "Script/Squirrel/API/SquirrelScriptAPI.h"
#include "Render/IRender.h"

void RegisterRenderAPI_Squirrel(Sqrat::RootTable& rootTable)
{
	HSQUIRRELVM vm = rootTable.GetVM();

	// Render
	rootTable.Bind("IRender", Sqrat::Class<IRender, Sqrat::NoConstructor<IRender>>(vm, "IRender")
	);
	rootTable.SetInstance("Render", IRender::Get());
}
