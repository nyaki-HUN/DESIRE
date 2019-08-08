#include "API/AngelScriptAPI.h"

#include "Engine/Modules.h"
#include "Engine/Sound/SoundSystem.h"

void RegisterSoundAPI_AngelScript(asIScriptEngine* engine)
{
	SoundSystem* soundSystem = Modules::SoundSystem.get();
	if(soundSystem == nullptr)
	{
		return;
	}

	int result = asSUCCESS;

	// SoundSystem
	result = engine->RegisterObjectType("ISoundSystem", 0, asOBJ_REF | asOBJ_NOHANDLE);					ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalProperty("ISoundSystem SoundSystem", soundSystem);					ASSERT(result >= asSUCCESS);
}
