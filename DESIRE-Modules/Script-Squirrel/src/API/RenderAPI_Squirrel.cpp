#include "API/SquirrelScriptAPI.h"

#include "Engine/Core/Modules.h"
#include "Engine/Render/Render.h"

void RegisterRenderAPI_Squirrel(Sqrat::RootTable& rootTable)
{
	Render *render = Modules::Render.get();
	if(render == nullptr)
	{
		return;
	}

	HSQUIRRELVM vm = rootTable.GetVM();

	// Render
	rootTable.Bind("IRender", Sqrat::Class<Render, Sqrat::NoConstructor<Render>>(vm, "IRender")
	);
	rootTable.SetInstance("Render", render);
}
