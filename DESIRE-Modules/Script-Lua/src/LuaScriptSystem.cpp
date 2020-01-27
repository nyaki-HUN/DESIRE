#include "stdafx_Lua.h"
#include "LuaScriptSystem.h"
#include "LuaScriptComponent.h"
#include "API/LuaScriptAPI.h"

#include "Engine/Core/FS/FileSystem.h"
#include "Engine/Core/FS/IReadFile.h"
#include "Engine/Core/Memory/MemorySystem.h"
#include "Engine/Core/Object.h"
#include "Engine/Core/String/StackString.h"

LuaScriptSystem::LuaScriptSystem()
{
	L = lua_newstate(&LuaScriptSystem::ReallocWrapper, this);
	lua_setallocf(L, &LuaScriptSystem::ReallocWrapper, this);
	lua_atpanic(L, &LuaScriptSystem::LuaPanicFunc);

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

	MemoryBuffer data = file->ReadFileContent();
	String content = data.AsString();
	luaL_loadbuffer(L, content.Str(), content.Length(), scriptName.Str());
	const int statusCode = lua_pcall(L, 0, LUA_MULTRET, 0);
	if(statusCode != LUA_OK)
	{
		LOG_ERROR("Could not compile script: %s", filename.Str());
		return;
	}
}

int LuaScriptSystem::LuaPanicFunc(lua_State* L)
{
	lua_writestringerror("PANIC: unprotected error in call to Lua API (%s)\n", lua_tostring(L, -1));
	return 0;	// Return to Lua to abort
}

void* LuaScriptSystem::ReallocWrapper(void* userData, void* ptr, size_t oldSize, size_t newSize)
{
	DESIRE_UNUSED(userData);
	DESIRE_UNUSED(oldSize);
	return MemorySystem::Realloc(ptr, newSize);
}
