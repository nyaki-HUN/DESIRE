#include "API/LuaScriptAPI.h"

#include "Engine/Core/Modules.h"
#include "Engine/Render/Render.h"

void RegisterRenderAPI_Lua(lua_State *L)
{
	Render *render = Modules::Render.get();
	if(render == nullptr)
	{
		return;
	}

	// Render
	luabridge::getGlobalNamespace(L).beginClass<Render>("IRender")
		.endClass();

	luabridge::setGlobal(L, render, "Render");
}
