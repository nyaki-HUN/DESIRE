#include "API/LuaScriptAPI.h"

#include "Engine/Sound/SoundSystem.h"

void RegisterSoundAPI_Lua(sol::state_view& lua)
{
	SoundSystem* soundSystem = Modules::SoundSystem.get();
	if(soundSystem == nullptr)
	{
		return;
	}

	// SoundSystem
	auto isoundsystem = lua.new_usertype<SoundSystem>("ISoundSystem");
	lua["SoundSystem"] = soundSystem;
}
