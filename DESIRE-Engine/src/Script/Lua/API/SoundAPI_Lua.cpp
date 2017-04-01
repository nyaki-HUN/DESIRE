#include "stdafx.h"
#include "Script/Lua/API/LuaScriptAPI.h"
#include "Sound/ISound.h"

void RegisterSoundAPI_Lua(lua_State *L)
{
	// Sound
	DESIRE_UNUSED(L);
//	luabridge::setGlobal(L, Sound::Get(), "Sound");
}
