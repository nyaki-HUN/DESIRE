#include "LuaScriptSystem.h"
#include "LuaScriptComponent.h"
#include "API/LuaScriptAPI.h"

#include "Engine/Core/Object.h"
#include "Engine/Core/FS/FileSystem.h"
#include "Engine/Core/FS/IReadFile.h"
#include "Engine/Core/Log/Log.h"
#include "Engine/Core/String/StackString.h"

#include "lua.hpp"

LuaScriptSystem::LuaScriptSystem()
{
	L = luaL_newstate();

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

	RegisterCoreAPI_Lua(L);
	RegisterInputAPI_Lua(L);
	RegisterNetworkAPI_Lua(L);
	RegisterPhysicsAPI_Lua(L);
	RegisterRenderAPI_Lua(L);
	RegisterSceneAPI_Lua(L);
	RegisterSoundAPI_Lua(L);

	// ScriptComponent
	luabridge::getGlobalNamespace(L).deriveClass<LuaScriptComponent, Component>("ScriptComponent")
		.addFunction("Call", &LuaScriptComponent::CallFromScript)
		.endClass();

	luabridge::getGlobalNamespace(L).beginClass<Object>("Object")
		.addFunction("GetScriptComponent", &Object::GetComponent<LuaScriptComponent>)
		.endClass();
}

LuaScriptSystem::~LuaScriptSystem()
{
	lua_close(L);
}

ScriptComponent* LuaScriptSystem::CreateScriptComponentOnObject_Internal(Object& object, const String& scriptName)
{
	lua_State* newL = lua_newthread(L);
	if(newL == nullptr)
	{
		LOG_ERROR("Failed to create new script thread");
		return nullptr;
	}

	CompileScript(scriptName, newL);

	LuaScriptComponent* scriptComponent = &object.AddComponent<LuaScriptComponent>(newL);
	return scriptComponent;
}

void LuaScriptSystem::CompileScript(const String& scriptName, lua_State* L)
{
	const StackString<DESIRE_MAX_PATH_LEN> filename = StackString<DESIRE_MAX_PATH_LEN>::Format("data/scripts/%s.lua", scriptName.Str());
	ReadFilePtr file = FileSystem::Get()->Open(filename);
	if(file == nullptr)
	{
		LOG_ERROR("Could not load script: %s", filename.Str());
		return;
	}

	MemoryBuffer content = file->ReadFileContent();
	luaL_loadbuffer(L, content.data, content.size - 1, scriptName.Str());
	const int statusCode = lua_pcall(L, 0, LUA_MULTRET, 0);
	ASSERT(statusCode == LUA_OK);
}
