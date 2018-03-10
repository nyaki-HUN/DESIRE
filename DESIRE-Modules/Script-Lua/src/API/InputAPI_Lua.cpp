#include "API/LuaScriptAPI.h"

#include "Engine/Core/Modules.h"
#include "Engine/Input/Input.h"

void RegisterInputAPI_Lua(lua_State *L)
{
	// Input
	luabridge::getGlobalNamespace(L).beginClass<Input>("IInput")
		.addFunction("GetOsMouseCursorPos", &Input::GetOsMouseCursorPos)
		.endClass();

	luabridge::setGlobal(L, Modules::Input.get(), "Input");
}
