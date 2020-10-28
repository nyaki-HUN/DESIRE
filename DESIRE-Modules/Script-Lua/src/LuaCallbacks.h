#pragma once

class LuaCallbacks
{
public:
	static void* ReallocWrapper(void* pUserData, void* pMemory, size_t oldSize, size_t newSize);

	static int LuaPanicFunc(lua_State* L);
};
