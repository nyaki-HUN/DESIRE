#include "stdafx_Lua.h"
#include "API/LuaScriptAPI.h"

#include "Engine/Sound/SoundSystem.h"

void RegisterSoundAPI_Lua(sol::state_view& lua)
{
	if(Modules::SoundSystem == nullptr)
	{
		return;
	}

	// SoundSystem
	auto isoundsystem = lua.new_usertype<SoundSystem>("ISoundSystem");
	lua["SoundSystem"] = Modules::SoundSystem.get();
}
