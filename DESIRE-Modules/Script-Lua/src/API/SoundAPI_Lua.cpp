#include "API/LuaScriptAPI.h"

#include "Engine/Modules.h"
#include "Engine/Sound/SoundSystem.h"

void RegisterSoundAPI_Lua(lua_State* L)
{
	SoundSystem* soundSystem = Modules::SoundSystem.get();
	if(soundSystem == nullptr)
	{
		return;
	}

	// SoundSystem
//	luabridge::setGlobal(L, soundSystem, "SoundSystem");
}
