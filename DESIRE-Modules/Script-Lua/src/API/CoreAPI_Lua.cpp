#include "API/LuaScriptAPI.h"

#include "Engine/Application/Application.h"

#include "Engine/Core/Component.h"
#include "Engine/Core/Math/Transform.h"
#include "Engine/Core/Object.h"
#include "Engine/Core/Timer.h"

void RegisterCoreAPI_Lua(sol::state_view& lua)
{
	// Component
	lua.new_usertype<Component>("Component",
		"enabled", sol::property(&Component::IsEnabled, &Component::SetEnabled),
		"object", sol::property(&Component::GetObject)
	);

	// Object
	lua.new_usertype<Object>("Object",
		"GetObjectName", &Object::GetObjectName,
		"SetActive", &Object::SetActive,
		"IsActiveSelf", &Object::IsActiveSelf,
		"IsActiveInHierarchy", &Object::IsActiveInHierarchy,
		"RemoveComponent", &Object::RemoveComponent,
		"GetComponent", &Object::GetComponentByTypeId,
		"transform", sol::property(&Object::GetTransform),
		"parent", sol::property(&Object::GetParent, &Object::SetParent)
	);

	// Timer
	lua.new_usertype<Timer>("ITimer",
		"GetTimeMicroSec", &Timer::GetTimeMicroSec,
		"GetTimeMilliSec", &Timer::GetTimeMilliSec,
		"GetTimeSec", &Timer::GetTimeSec,
		"GetMicroDelta", &Timer::GetMicroDelta,
		"GetMilliDelta", &Timer::GetMilliDelta,
		"GetSecDelta", &Timer::GetSecDelta);

	lua.set("Timer", Modules::Application->GetTimer());
}
