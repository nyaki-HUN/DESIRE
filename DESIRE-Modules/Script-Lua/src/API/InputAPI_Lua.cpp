#include "API/LuaScriptAPI.h"

#include "Engine/Input/Input.h"

void RegisterInputAPI_Lua(sol::state_view& lua)
{
	Input* input = Modules::Input.get();
	if(input == nullptr)
	{
		return;
	}

	// Input
	lua.new_usertype<Input>("IInput",
		"GetOsMouseCursorPos", &Input::GetOsMouseCursorPos
	);

	lua.set("Input", input);
}
