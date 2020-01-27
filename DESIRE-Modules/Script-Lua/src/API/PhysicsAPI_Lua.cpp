#include "stdafx_Lua.h"
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
	auto physicsComponent = lua.new_usertype<PhysicsComponent>("PhysicsComponent", sol::base_classes, sol::bases<Component>());
	physicsComponent.set("mass", sol::property(&PhysicsComponent::GetMass, &PhysicsComponent::SetMass));

	lua["Object"]["GetPhysicsComponent"] = &Object::GetComponent<PhysicsComponent>;

	// Physics
	auto iphysics = lua.new_usertype<Physics>("IPhysics");
	iphysics.set_function("RaycastAny", &Physics::RaycastAny);
	lua["Physics"] = physics;
}
