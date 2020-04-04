#include "Engine/stdafx.h"

#if DESIRE_PLATFORM_WINDOWS

#include "Engine/Application/Application.h"

#include "Engine/Core/WINDOWS/os.h"

#include <crtdbg.h>

int WINAPI WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
#if defined(_DEBUG)
	int debugFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	debugFlag |= _CRTDBG_ALLOC_MEM_DF;
	debugFlag |= _CRTDBG_LEAK_CHECK_DF;
//	debugFlag |= _CRTDBG_CHECK_ALWAYS_DF;
	debugFlag |= _CRTDBG_DELAY_FREE_MEM_DF;
	_CrtSetDbgFlag(debugFlag);
//	_CrtSetBreakAlloc(123);
#endif

	return Application::Start(__argc, __argv);
}

#endif	// #if DESIRE_PLATFORM_WINDOWS
