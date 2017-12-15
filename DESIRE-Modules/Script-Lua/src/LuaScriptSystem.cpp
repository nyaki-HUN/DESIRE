#include "stdafx.h"
#include "LuaScriptSystem.h"
#include "LuaScriptComponent.h"
#include "API/LuaScriptAPI.h"

#include "Core/fs/FileSystem.h"
#include "Core/fs/IReadFile.h"

#include "lua.hpp"

LuaScriptSystem::LuaScriptSystem()
	: L(nullptr)
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
	RegisterComponentAPI_Lua(L);
	RegisterInputAPI_Lua(L);
	RegisterNetworkAPI_Lua(L);
	RegisterPhysicsAPI_Lua(L);
	RegisterRenderAPI_Lua(L);
	RegisterSoundAPI_Lua(L);
}

LuaScriptSystem::~LuaScriptSystem()
{
	lua_close(L);
}

ScriptComponent* LuaScriptSystem::CreateScriptComponent_Internal(const char *scriptName)
{
	ASSERT(scriptName != nullptr);

	lua_State *newL = lua_newthread(L);
	if(newL == nullptr)
	{
		LOG_ERROR("Failed to create new script thread");
		return nullptr;
	}

	CompileScript(scriptName, newL);

	LuaScriptComponent *scriptComponent = new LuaScriptComponent(newL);
	luabridge::setGlobal(newL, scriptComponent, "self");

	return scriptComponent;
}

void LuaScriptSystem::CompileScript(const char *scriptName, lua_State *L)
{
	char filename[DESIRE_MAX_PATH_LEN];
	snprintf(filename, sizeof(filename), "data/scripts/%s.lua", scriptName);
	ReadFilePtr file = FileSystem::Get()->Open(filename);
	if(file == nullptr)
	{
		LOG_ERROR("Could not load script: %s", filename);
		return;
	}

	MemoryBuffer content = file->ReadFileContent();
	luaL_loadbuffer(L, content.data, content.size, scriptName);
	const int statusCode = lua_pcall(L, 0, LUA_MULTRET, 0);
	ASSERT(statusCode == LUA_OK);
}
