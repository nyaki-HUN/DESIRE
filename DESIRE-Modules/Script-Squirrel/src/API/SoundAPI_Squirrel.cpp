#include "stdafx_Squirrel.h"
#include "API/SquirrelScriptAPI.h"

#include "Engine/Sound/SoundSystem.h"

void RegisterSoundAPI_Squirrel(Sqrat::RootTable& rootTable)
{
	if(Modules::SoundSystem == nullptr)
	{
		return;
	}

	HSQUIRRELVM vm = rootTable.GetVM();

	// SoundSystem
	rootTable.Bind("ISoundSystem", Sqrat::Class<SoundSystem, Sqrat::NoConstructor<SoundSystem>>(vm, "ISoundSystem")
	);
	rootTable.SetInstance("SoundSystem", Modules::SoundSystem.get());
}
