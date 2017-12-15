#include "stdafx.h"
#include "Script/AngelScript/API/AngelScriptAPI.h"
#include "Sound/ISound.h"

void RegisterSoundAPI_AngelScript(asIScriptEngine *engine)
{
	// Sound
	engine->RegisterObjectType("ISound", 0, asOBJ_REF | asOBJ_NOHANDLE);
//	engine->RegisterGlobalProperty("ISound Sound", ISound::Get());
}
