#include "API/SquirrelScriptAPI.h"

#include "Engine/Core/Modules.h"
#include "Engine/Physics/Physics.h"
#include "Engine/Physics/PhysicsComponent.h"

void RegisterPhysicsAPI_Squirrel(Sqrat::RootTable& rootTable)
{
	Physics *physics = Modules::Physics.get();
	if(physics == nullptr)
	{
		return;
	}

	HSQUIRRELVM vm = rootTable.GetVM();

	// PhysicsComponent
	rootTable.Bind("PhysicsComponent", Sqrat::DerivedClass<PhysicsComponent, Component, Sqrat::NoConstructor<PhysicsComponent>>(vm, "PhysicsComponent")
		.Prop("mass", &PhysicsComponent::GetMass, &PhysicsComponent::SetMass)
	);

	// Physics
	rootTable.Bind("IPhysics", Sqrat::Class<Physics, Sqrat::NoConstructor<Physics>>(vm, "IPhysics")
		.Func("RaycastAny", &Physics::RaycastAny)
	);
	rootTable.SetInstance("Physics", physics);
}
