#pragma once

#include "lua/include/lua.hpp"
#include "Script/Lua/LuaBridge/LuaBridge.h"

// Engine API register functions
void RegisterComponentAPI_Lua(lua_State *L);
void RegisterCoreAPI_Lua(lua_State *L);
void RegisterInputAPI_Lua(lua_State *L);
void RegisterNetworkAPI_Lua(lua_State *L);
void RegisterPhysicsAPI_Lua(lua_State *L);
void RegisterRenderAPI_Lua(lua_State *L);
void RegisterSoundAPI_Lua(lua_State *L);

template<class T>
class LuaScriptAPI
{
public:

};
