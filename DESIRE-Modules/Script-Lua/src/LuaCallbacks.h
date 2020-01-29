#pragma once

class LuaCallbacks
{
public:
	static void* ReallocWrapper(void* userData, void* ptr, size_t oldSize, size_t newSize);

	static int LuaPanicFunc(lua_State* L);
};
