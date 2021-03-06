#include "stdafx_Squirrel.h"
#include "API/SquirrelScriptAPI.h"

#include "Engine/Core/Object.h"

#include "Engine/Physics/Physics.h"
#include "Engine/Physics/PhysicsComponent.h"

void RegisterPhysicsAPI_Squirrel(Sqrat::RootTable& rootTable)
{
	if(Modules::Physics == nullptr)
	{
		return;
	}

	HSQUIRRELVM vm = rootTable.GetVM();

	// PhysicsComponent
	rootTable.Bind("PhysicsComponent", Sqrat::DerivedClass<PhysicsComponent, Component, Sqrat::NoConstructor<PhysicsComponent>>(vm, "PhysicsComponent")
		.Func("SetMass", &PhysicsComponent::SetMass)
		.Func("GetMass", &PhysicsComponent::GetMass)
	);

	Sqrat::Class<Object, Sqrat::NoConstructor<Object>>(vm, "Object", false).Func("GetPhysicsComponent", &Object::GetComponent<PhysicsComponent>);

	// Physics
	rootTable.Bind("IPhysics", Sqrat::Class<Physics, Sqrat::NoConstructor<Physics>>(vm, "IPhysics")
		.Func("RaycastAny", &Physics::RaycastAny)
	);
	rootTable.SetInstance("Physics", Modules::Physics.get());
}
