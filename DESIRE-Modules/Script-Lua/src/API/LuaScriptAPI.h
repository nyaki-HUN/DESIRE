#pragma once

#include "Engine/Core/String/String.h"

// Engine API register functions
void RegisterCoreAPI_Math_Lua(sol::state_view& lua);
void RegisterCoreAPI_Lua(sol::state_view& lua);
void RegisterInputAPI_Lua(sol::state_view& lua);
void RegisterPhysicsAPI_Lua(sol::state_view& lua);
void RegisterRenderAPI_Lua(sol::state_view& lua);
void RegisterSoundAPI_Lua(sol::state_view& lua);

template<class T>
class LuaAPI
{
public:
	template<const String& (T::*func)() const>
	static const char* MakeStringRvFrom(T* pThis)
	{
		const String& rv = (pThis->*func)();
		return rv.Str();
	}
};
