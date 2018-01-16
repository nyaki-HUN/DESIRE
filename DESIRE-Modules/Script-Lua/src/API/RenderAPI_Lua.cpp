#include "API/LuaScriptAPI.h"

#include "Engine/Render/Render.h"

void RegisterRenderAPI_Lua(lua_State *L)
{
	// Render
	luabridge::getGlobalNamespace(L).beginClass<Render>("IRender")
		.endClass();

	luabridge::setGlobal(L, Render::Get(), "Render");
}
