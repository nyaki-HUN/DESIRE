#include "stdafx_Lua.h"
#include "API/LuaScriptAPI.h"

#include "Engine/Application/Application.h"

#include "Engine/Core/Component.h"
#include "Engine/Core/Math/Transform.h"
#include "Engine/Core/Object.h"
#include "Engine/Core/Timer.h"

void RegisterCoreAPI_Lua(sol::state_view& lua)
{
	// Component
	auto component = lua.new_usertype<Component>("Component");
	component.set("enabled", sol::property(&Component::IsEnabled, &Component::SetEnabled));
	component.set("object", sol::property(&Component::GetObject));

	// Object
	auto object = lua.new_usertype<Object>("Object");
	object.set_function("GetObjectName", &Object::GetObjectName);
	object.set_function("SetActive", &Object::SetActive);
	object.set_function("IsActiveSelf", &Object::IsActiveSelf);
	object.set_function("IsActiveInHierarchy", &Object::IsActiveInHierarchy);
	object.set_function("RemoveComponent", &Object::RemoveComponent);
	object.set_function("GetComponent", &Object::GetComponentByTypeId);
	object.set("transform", sol::property(&Object::GetTransform));
	object.set("parent", sol::property(&Object::GetParent, &Object::SetParent));

	// Timer
	auto timer = lua.new_usertype<Timer>("ITimer");
	timer.set_function("GetTimeMicroSec", &Timer::GetTimeMicroSec);
	timer.set_function("GetTimeMilliSec", &Timer::GetTimeMilliSec);
	timer.set_function("GetTimeSec", &Timer::GetTimeSec);
	timer.set_function("GetMicroDelta", &Timer::GetMicroDelta);
	timer.set_function("GetMilliDelta", &Timer::GetMilliDelta);
	timer.set_function("GetSecDelta", &Timer::GetSecDelta);
	lua["Timer"] = Modules::Application->GetTimer();
}
