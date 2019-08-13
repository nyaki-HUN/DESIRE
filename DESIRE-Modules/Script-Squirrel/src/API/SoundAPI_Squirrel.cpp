#include "API/SquirrelScriptAPI.h"

#include "Engine/Modules.h"
#include "Engine/Sound/SoundSystem.h"

void RegisterSoundAPI_Squirrel(Sqrat::RootTable& rootTable)
{
	SoundSystem* soundSystem = Modules::SoundSystem.get();
	if(soundSystem == nullptr)
	{
		return;
	}

	HSQUIRRELVM vm = rootTable.GetVM();

	// SoundSystem
	rootTable.Bind("ISoundSystem", Sqrat::Class<SoundSystem, Sqrat::NoConstructor<SoundSystem>>(vm, "ISoundSystem")
		.Func("PlaySound", &SoundSystem::PlaySound)
	);
	rootTable.SetInstance("SoundSystem", soundSystem);
}
