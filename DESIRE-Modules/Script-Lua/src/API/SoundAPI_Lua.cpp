#include "stdafx.h"
#include "API/LuaScriptAPI.h"

#include "Sound/Sound.h"

void RegisterSoundAPI_Lua(lua_State *L)
{
	// Sound
	DESIRE_UNUSED(L);
//	luabridge::setGlobal(L, Sound::Get(), "Sound");
}
