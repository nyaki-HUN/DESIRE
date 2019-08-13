#include "API/LuaScriptAPI.h"

#include "Engine/Modules.h"
#include "Engine/Input/Input.h"

void RegisterInputAPI_Lua(lua_State* L)
{
	Input* input = Modules::Input.get();
	if(input == nullptr)
	{
		return;
	}

	// Input
	luabridge::getGlobalNamespace(L).beginClass<Input>("IInput")
		.addFunction("GetOsMouseCursorPos", &Input::GetOsMouseCursorPos)
		.endClass();

	luabridge::setGlobal(L, input, "Input");
}
