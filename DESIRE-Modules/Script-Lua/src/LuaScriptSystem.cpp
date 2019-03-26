#include "LuaScriptSystem.h"
#include "LuaScriptComponent.h"
#include "API/LuaScriptAPI.h"

#include "Engine/Core/Log.h"
#include "Engine/Core/fs/FileSystem.h"
#include "Engine/Core/fs/IReadFile.h"
#include "Engine/Core/String/StackString.h"
#include "Engine/Scene/Object.h"

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

ScriptComponent* LuaScriptSystem::CreateScriptComponentOnObject_Internal(Object& object, const char *scriptName)
{
	ASSERT(scriptName != nullptr);

	lua_State *newL = lua_newthread(L);
	if(newL == nullptr)
	{
		LOG_ERROR("Failed to create new script thread");
		return nullptr;
	}

	CompileScript(scriptName, newL);

	LuaScriptComponent& scriptComponent = object.AddComponent<LuaScriptComponent>(newL);
	return &scriptComponent;
}

void LuaScriptSystem::CompileScript(const char *scriptName, lua_State *L)
{
	const StackString<DESIRE_MAX_PATH_LEN> filename = StackString<DESIRE_MAX_PATH_LEN>::Format("data/scripts/%s.lua", scriptName);
	ReadFilePtr file = FileSystem::Get()->Open(filename);
	if(file == nullptr)
	{
		LOG_ERROR("Could not load script: %s", filename.Str());
		return;
	}

	MemoryBuffer content = file->ReadFileContent();
	luaL_loadbuffer(L, content.data, content.size, scriptName);
	const int statusCode = lua_pcall(L, 0, LUA_MULTRET, 0);
	ASSERT(statusCode == LUA_OK);
}
