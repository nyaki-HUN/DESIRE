#include "stdafx.h"
#include "Script/Squirrel/API/SquirrelScriptAPI.h"
#include "Sound/ISound.h"

void RegisterSoundAPI_Squirrel(Sqrat::RootTable& rootTable)
{
	HSQUIRRELVM vm = rootTable.GetVM();

	// Sound
	DESIRE_UNUSED(vm);
//	rootTable.SetInstance("Sound", Sound::Get());
}
