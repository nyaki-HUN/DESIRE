#include "API/AngelScriptAPI.h"

void RegisterNetworkAPI_AngelScript(asIScriptEngine* engine)
{
	int result = asSUCCESS;

	// Network
	result = engine->RegisterObjectType("INetwork", 0, asOBJ_REF | asOBJ_NOHANDLE);					ASSERT(result >= asSUCCESS);
}
