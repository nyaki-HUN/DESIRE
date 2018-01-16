#include "API/AngelScriptAPI.h"

#include "Engine/Sound/Sound.h"

void RegisterSoundAPI_AngelScript(asIScriptEngine *engine)
{
	// Sound
	engine->RegisterObjectType("ISound", 0, asOBJ_REF | asOBJ_NOHANDLE);
	engine->RegisterGlobalProperty("ISound Sound", Sound::Get());
}
