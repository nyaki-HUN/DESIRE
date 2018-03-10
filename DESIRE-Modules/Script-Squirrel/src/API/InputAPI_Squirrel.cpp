#include "API/SquirrelScriptAPI.h"

#include "Engine/Core/Modules.h"
#include "Engine/Input/Input.h"

void RegisterInputAPI_Squirrel(Sqrat::RootTable& rootTable)
{
	HSQUIRRELVM vm = rootTable.GetVM();

	// Input
	rootTable.Bind("IInput", Sqrat::Class<Input, Sqrat::NoConstructor<Input>>(vm, "IInput")
		.Func("GetOsMouseCursorPos", &Input::GetOsMouseCursorPos)
	);
	rootTable.SetInstance("Input", Modules::Input.get());
}
