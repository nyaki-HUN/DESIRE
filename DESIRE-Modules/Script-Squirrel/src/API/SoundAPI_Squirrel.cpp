#include "API/SquirrelScriptAPI.h"

#include "Engine/Sound/Sound.h"

void RegisterSoundAPI_Squirrel(Sqrat::RootTable& rootTable)
{
	HSQUIRRELVM vm = rootTable.GetVM();

	// Sound
	rootTable.Bind("ISound", Sqrat::Class<Sound, Sqrat::NoConstructor<Sound>>(vm, "IPhysics")
		.Func("PlaySound", &Sound::PlaySound)
	);
	rootTable.SetInstance("Sound", Sound::Get());
}
