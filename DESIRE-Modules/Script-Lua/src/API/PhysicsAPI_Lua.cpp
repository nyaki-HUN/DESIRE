#include "API/LuaScriptAPI.h"

#include "Engine/Core/Object.h"

#include "Engine/Physics/Physics.h"
#include "Engine/Physics/PhysicsComponent.h"

void RegisterPhysicsAPI_Lua(sol::state_view& lua)
{
	Physics* physics = Modules::Physics.get();
	if(physics == nullptr)
	{
		return;
	}

	// PhysicsComponent
	lua.new_usertype<PhysicsComponent>("PhysicsComponent",
		sol::base_classes, sol::bases<Component>(),
		"mass", sol::property(&PhysicsComponent::GetMass, &PhysicsComponent::SetMass)
	);

	lua["Object"]["GetPhysicsComponent"] = &Object::GetComponent<PhysicsComponent>;

	// Physics
	lua.new_usertype<Physics>("IPhysics",
		"RaycastAny", &Physics::RaycastAny
	);
	lua.set("Physics", physics);
}
