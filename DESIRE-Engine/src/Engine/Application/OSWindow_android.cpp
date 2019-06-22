#include "Engine/stdafx.h"

#if DESIRE_PLATFORM_ANDROID

#include "Engine/Application/OSWindow.h"
#include "Engine/Core/String/WritableString.h"

#include <android_native_app_glue.h>

// --------------------------------------------------------------------------------------------------------------------
//	OSWindowImpl
// --------------------------------------------------------------------------------------------------------------------

class OSWindowImpl
{
public:
	ANativeWindow *nativeWindow = nullptr;
};

// --------------------------------------------------------------------------------------------------------------------
//	OSWindow
// --------------------------------------------------------------------------------------------------------------------

OSWindow::OSWindow(const OSWindowCreationParams& creationParams)
	: width(std::max(kWindowMinSize, creationParams.width))
	, height(std::max(kWindowMinSize, creationParams.height))
	, isFullscreen(creationParams.isFullscreen)
	, impl(std::make_unique<OSWindowImpl>())
{

}

OSWindow::~OSWindow()
{

}

void OSWindow::HandleWindowMessages()
{

}

void* OSWindow::GetHandle() const
{
	return impl->nativeWindow;
}

void OSWindow::SetCursor(ECursor cursor)
{

}

bool OSWindow::SetClipboardString(const String& string)
{
	return false;
}

void OSWindow::GetClipboardString(WritableString& outString)
{
	outString.Clear();
}

#endif	// #if DESIRE_PLATFORM_ANDROID
