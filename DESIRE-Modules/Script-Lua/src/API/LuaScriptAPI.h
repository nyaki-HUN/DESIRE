#pragma once

#include "Engine/Modules.h"

#include "Engine/Core/String/String.h"

#include "lua.hpp"
#include "LuaBridge/LuaBridge.h"

// Engine API register functions
void RegisterCoreAPI_Lua(lua_State* L);
void RegisterInputAPI_Lua(lua_State* L);
void RegisterNetworkAPI_Lua(lua_State* L);
void RegisterPhysicsAPI_Lua(lua_State* L);
void RegisterRenderAPI_Lua(lua_State* L);
void RegisterSceneAPI_Lua(lua_State* L);
void RegisterSoundAPI_Lua(lua_State* L);

template<>
struct luabridge::Stack<const String&>
{
	static inline void push(lua_State* L, const String& str)
	{
		lua_pushlstring(L, str.Str(), str.Length());
	}
};

template<class T>
class LuaScriptAPI
{
public:

};
