#include "stdafx.h"
#include "Script/Lua/API/LuaScriptAPI.h"
#include "Physics/IPhysics.h"

void RegisterPhysicsAPI_Lua(lua_State *L)
{
	// Physics
	DESIRE_UNUSED(L);
//	luabridge::setGlobal(L, Physics::Get(), "Physics");
}
