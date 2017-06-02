#include "stdafx.h"
#include "Script/Lua/API/LuaScriptAPI.h"
#include "Render/IRender.h"

void RegisterRenderAPI_Lua(lua_State *L)
{
	// Render
	DESIRE_UNUSED(L);
	luabridge::setGlobal(L, IRender::Get(), "Render");
}
