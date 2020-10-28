#include "stdafx_AngelScript.h"
#include "API/AngelScriptAPI.h"

#include "Engine/Sound/SoundSystem.h"

void RegisterSoundAPI_AngelScript(asIScriptEngine& engine)
{
	if(Modules::SoundSystem == nullptr)
	{
		return;
	}

	int result = asSUCCESS;

	// SoundSystem
	result = engine.RegisterObjectType("ISoundSystem", 0, asOBJ_REF | asOBJ_NOHANDLE);				ASSERT(result >= asSUCCESS);
	result = engine.RegisterGlobalProperty("ISoundSystem SoundSystem", Modules::SoundSystem.get());	ASSERT(result >= asSUCCESS);
}
