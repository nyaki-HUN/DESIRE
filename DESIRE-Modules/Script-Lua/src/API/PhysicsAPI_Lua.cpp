#include "API/LuaScriptAPI.h"

#include "Engine/Core/Modules.h"
#include "Engine/Physics/Physics.h"
#include "Engine/Physics/PhysicsComponent.h"

void RegisterPhysicsAPI_Lua(lua_State *L)
{
	Physics *physics = Modules::Physics.get();
	if(physics == nullptr)
	{
		return;
	}

	// PhysicsComponent
	luabridge::getGlobalNamespace(L).deriveClass<PhysicsComponent, Component>("PhysicsComponent")
		.addProperty("mass", &PhysicsComponent::GetMass, &PhysicsComponent::SetMass)
		.endClass();

	// Physics
	luabridge::getGlobalNamespace(L).beginClass<Physics>("IPhysics")
		.addFunction("RaycastAny", &Physics::RaycastAny)
		.endClass();

	luabridge::setGlobal(L, physics, "Physics");
}
