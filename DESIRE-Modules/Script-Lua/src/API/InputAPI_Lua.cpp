#include "stdafx_Lua.h"
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
	auto iinput = lua.new_usertype<Input>("IInput");
	iinput.set_function("GetOsMouseCursorPos", &Input::GetOsMouseCursorPos);
	lua["Input"] = input;
}
