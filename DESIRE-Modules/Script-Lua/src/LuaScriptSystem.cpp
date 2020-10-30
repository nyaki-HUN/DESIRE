#include "stdafx_Lua.h"
#include "LuaScriptSystem.h"

#include "API/LuaScriptAPI.h"
#include "LuaScriptComponent.h"
#include "LuaCallbacks.h"

#include "Engine/Core/FS/FileSystem.h"
#include "Engine/Core/Object.h"
#include "Engine/Core/String/StackString.h"

LuaScriptSystem::LuaScriptSystem()
{
	L = lua_newstate(&LuaCallbacks::ReallocWrapper, this);
	lua_setallocf(L, &LuaCallbacks::ReallocWrapper, this);
	lua_atpanic(L, &LuaCallbacks::LuaPanicFunc);

	const luaL_Reg luaLibsToLoad[] =
	{
		{ "_G", luaopen_base },
		{ LUA_COLIBNAME, luaopen_coroutine },
		{ LUA_TABLIBNAME, luaopen_table },
		{ LUA_STRLIBNAME, luaopen_string },
		{ LUA_MATHLIBNAME, luaopen_math },
		{ LUA_UTF8LIBNAME, luaopen_utf8 },
		{ LUA_DBLIBNAME, luaopen_debug },
	};

	for(const luaL_Reg& lib : luaLibsToLoad)
	{
		luaL_requiref(L, lib.name, lib.func, 1);
		lua_pop(L, 1);	// remove copy
	}

	sol::state_view lua(L);

	RegisterCoreAPI_Math_Lua(lua);
	RegisterCoreAPI_Lua(lua);
	RegisterInputAPI_Lua(lua);
	RegisterPhysicsAPI_Lua(lua);
	RegisterRenderAPI_Lua(lua);
	RegisterSoundAPI_Lua(lua);

	// ScriptComponent
	auto scriptComponent = lua.new_usertype<LuaScriptComponent>("ScriptComponent", sol::base_classes, sol::bases<Component>());
	scriptComponent.set_function("Call", &LuaScriptComponent::CallFromScript);

	lua["Object"]["GetScriptComponent"] = &Object::GetComponent<LuaScriptComponent>;
}

LuaScriptSystem::~LuaScriptSystem()
{
	lua_close(L);
}

ScriptComponent* LuaScriptSystem::CreateScriptComponentOnObject_Internal(Object& object, const String& scriptName)
{
	lua_State* pNewL = lua_newthread(L);
	if(pNewL == nullptr)
	{
		LOG_ERROR("Failed to create new script thread");
		return nullptr;
	}

	CompileScript(scriptName, pNewL);

	LuaScriptComponent& scriptComponent = object.AddComponent<LuaScriptComponent>(pNewL);
	return &scriptComponent;
}

void LuaScriptSystem::CompileScript(const String& scriptName, lua_State* L)
{
	const StackString<DESIRE_MAX_PATH_LEN> filename = StackString<DESIRE_MAX_PATH_LEN>::Format("data/scripts/%s.lua", scriptName.Str());
	DynamicString data = FileSystem::Get()->LoadTextFile(filename);
	luaL_loadbuffer(L, data.Str(), data.Length(), scriptName.Str());
	const int statusCode = lua_pcall(L, 0, LUA_MULTRET, 0);
	if(statusCode != LUA_OK)
	{
		LOG_ERROR("Could not compile script: %s", filename.Str());
		return;
	}
}
