#include "stdafx_Squirrel.h"
#include "API/SquirrelScriptAPI.h"

#include "Engine/Application/Application.h"

#include "Engine/Core/Component.h"
#include "Engine/Core/Math/Transform.h"
#include "Engine/Core/Object.h"
#include "Engine/Core/Timer.h"

template<>
struct Sqrat::Var<const String&>
{
	static void push(HSQUIRRELVM vm, const String& string)
	{
		sq_pushstring(vm, string.Str(), string.Length());
	}
};

void RegisterCoreAPI_Squirrel(Sqrat::RootTable& rootTable)
{
	HSQUIRRELVM vm = rootTable.GetVM();

	// Component
	rootTable.Bind("Component", Sqrat::Class<Component, Sqrat::NoConstructor<Component>>(vm, "Component")
		.Func("SetEnabled", &Component::SetEnabled)
		.Func("IsEnabled", &Component::IsEnabled)
		.Func("GetObject", &Component::GetObject)
	);

	// Object
	rootTable.Bind("Object", Sqrat::Class<Object, Sqrat::NoConstructor<Object>>(vm, "Object")
		.Func("GetObjectName", &Object::GetObjectName)
		.Func("SetActive", &Object::SetActive)
		.Func("IsActiveSelf", &Object::IsActiveSelf)
		.Func("IsActiveInHierarchy", &Object::IsActiveInHierarchy)
		.Func("SetParent", &Object::SetParent)
		.Func("GetParent", &Object::GetParent)
		.Func("RemoveComponent", &Object::RemoveComponent)
		.Func("GetComponent", &Object::GetComponentByTypeId)
		.Func("GetTransform", &Object::GetTransform)
	);

	// Timer
	rootTable.Bind("ITimer", Sqrat::Class<Timer, Sqrat::NoConstructor<Timer>>(vm, "ITimer")
		.Func("GetTimeMicroSec", &Timer::GetTimeMicroSec)
		.Func("GetTimeMilliSec", &Timer::GetTimeMilliSec)
		.Func("GetTimeSec", &Timer::GetTimeSec)
		.Func("GetMicroDelta", &Timer::GetMicroDelta)
		.Func("GetMilliDelta", &Timer::GetMilliDelta)
		.Func("GetSecDelta", &Timer::GetSecDelta)
	);
	rootTable.SetInstance("Timer", Modules::Application->GetTimer());
}
