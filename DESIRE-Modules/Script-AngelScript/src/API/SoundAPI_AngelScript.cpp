#include "API/AngelScriptAPI.h"

#include "Engine/Core/Modules.h"
#include "Engine/Sound/SoundSystem.h"

void RegisterSoundAPI_AngelScript(asIScriptEngine *engine)
{
	// SoundSystem
	engine->RegisterObjectType("ISoundSystem", 0, asOBJ_REF | asOBJ_NOHANDLE);
	engine->RegisterGlobalProperty("ISoundSystem SoundSystem", Modules::SoundSystem.get());
}
