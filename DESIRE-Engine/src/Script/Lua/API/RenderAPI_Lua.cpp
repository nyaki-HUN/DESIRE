#include "stdafx.h"
#include "Script/Lua/API/LuaScriptAPI.h"
#include "Render/Render.h"

void RegisterRenderAPI_Lua(lua_State *L)
{
	// Render
	DESIRE_UNUSED(L);
//	luabridge::setGlobal(L, Render::Get(), "Render");
}
