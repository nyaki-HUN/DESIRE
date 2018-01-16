#include "API/SquirrelScriptAPI.h"

#include "Engine/Render/Render.h"

void RegisterRenderAPI_Squirrel(Sqrat::RootTable& rootTable)
{
	HSQUIRRELVM vm = rootTable.GetVM();

	// Render
	rootTable.Bind("IRender", Sqrat::Class<Render, Sqrat::NoConstructor<Render>>(vm, "IRender")
	);
	rootTable.SetInstance("Render", Render::Get());
}
