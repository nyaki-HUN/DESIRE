#include "stdafx.h"
#include "API/LuaScriptAPI.h"

#include "Input/Input.h"

void RegisterInputAPI_Lua(lua_State *L)
{
	// Input
	luabridge::getGlobalNamespace(L).beginClass<Input>("IInput")
		.addFunction("GetOsMouseCursorPos", &Input::GetOsMouseCursorPos)
		.endClass();

	luabridge::setGlobal(L, Input::Get(), "Input");
}
