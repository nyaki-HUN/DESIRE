#include "stdafx.h"
#include "API/LuaScriptAPI.h"

#include "Physics/Physics.h"

void RegisterPhysicsAPI_Lua(lua_State *L)
{
	// Physics
	luabridge::getGlobalNamespace(L).beginClass<Physics>("IPhysics")
		.addFunction("RaycastAny", &Physics::RaycastAny)
		.endClass();

	luabridge::setGlobal(L, Physics::Get(), "Physics");
}
