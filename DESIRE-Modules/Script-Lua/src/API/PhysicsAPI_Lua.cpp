#include "API/LuaScriptAPI.h"

#include "Engine/Core/Modules.h"
#include "Engine/Core/math/Vector3.h"
#include "Engine/Physics/Physics.h"

void RegisterPhysicsAPI_Lua(lua_State *L)
{
	Physics *physics = Modules::Physics.get();
	if(physics == nullptr)
	{
		return;
	}

	// Physics
	luabridge::getGlobalNamespace(L).beginClass<Physics>("IPhysics")
		.addFunction("RaycastAny", &Physics::RaycastAny)
		.endClass();

	luabridge::setGlobal(L, physics, "Physics");
}
