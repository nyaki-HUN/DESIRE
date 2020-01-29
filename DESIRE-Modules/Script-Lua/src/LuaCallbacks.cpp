#include "stdafx_Lua.h"
#include "LuaCallbacks.h"

#include "Engine/Core/Memory/MemorySystem.h"

void* LuaCallbacks::ReallocWrapper(void* userData, void* ptr, size_t oldSize, size_t newSize)
{
	DESIRE_UNUSED(userData);
	DESIRE_UNUSED(oldSize);
	return MemorySystem::Realloc(ptr, newSize);
}

int LuaCallbacks::LuaPanicFunc(lua_State* L)
{
	lua_writestringerror("PANIC: unprotected error in call to Lua API (%s)\n", lua_tostring(L, -1));
	return 0;	// Return to Lua to abort
}
