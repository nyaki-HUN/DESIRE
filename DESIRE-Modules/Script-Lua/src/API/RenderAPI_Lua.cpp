#include "stdafx.h"
#include "API/LuaScriptAPI.h"

#include "Render/IRender.h"

void RegisterRenderAPI_Lua(lua_State *L)
{
	// Render
	luabridge::getGlobalNamespace(L).beginClass<IRender>("IRender")
		.endClass();

	luabridge::setGlobal(L, IRender::Get(), "Render");
}
