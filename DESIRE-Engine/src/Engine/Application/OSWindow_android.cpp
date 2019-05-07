#include "Engine/stdafx.h"

#if defined(DESIRE_PLATFORM_ANDROID)

#include "Engine/Application/OSWindow.h"
#include "Engine/Application/Application.h"
#include "Engine/Application/EAppEventType.h"
#include "Engine/Core/Modules.h"
#include "Engine/Core/String/WritableString.h"

#include <android_native_app_glue.h>

// --------------------------------------------------------------------------------------------------------------------
//	OSWindowImpl
// --------------------------------------------------------------------------------------------------------------------

class OSWindowImpl
{
public:
	static void HandleOnAppCmd(android_app *androidApp, int32_t cmd)
	{
		switch(cmd)
		{
			// Command from main thread: the AInputQueue has changed.
			// Upon processing this command, android_app->inputQueue will be updated to the new queue (or NULL).
			case APP_CMD_INPUT_CHANGED:
				break;

			// Command from main thread: a new ANativeWindow is ready for use.
			// Upon receiving this command, android_app->window will contain the new window surface.
			case APP_CMD_INIT_WINDOW:
				if(androidApp->window != nullptr)
				{
					int32_t width = ANativeWindow_getWidth(androidApp->window);
					int32_t height = ANativeWindow_getHeight(androidApp->window);

					OSWindowCreationParams params;
					params.posX = 0;
					params.posY = 0;
					params.width = (uint16_t)width;
					params.height = (uint16_t)height;
					params.isFullscreen = true;

					OSWindow *window = new OSWindow(params);
					window->impl->nativeWindow = androidApp->window;
					androidApp->userData = window;
				}
				break;

			// Command from main thread: the existing ANativeWindow needs to be terminated.
			// Upon receiving this command, android_app->window still contains the existing window; after calling android_app_exec_cmd it will be set to NULL.
			case APP_CMD_TERM_WINDOW:
				break;

			// Command from main thread: the current ANativeWindow has been resized. Please redraw with its new size.
			case APP_CMD_WINDOW_RESIZED:
				if(androidApp->window != nullptr && androidApp->userData != nullptr)
				{
					int32_t width = ANativeWindow_getWidth(androidApp->window);
					int32_t height = ANativeWindow_getHeight(androidApp->window);
					ANDROIDWindow *window = reinterpret_cast<ANDROIDWindow*>(androidApp->userData);
					window->SetSize((uint16_t)width, (uint16_t)height);
				}
				break;

			// Command from main thread: the system needs that the current ANativeWindow be redrawn.
			// You should redraw the window before handing this to android_app_exec_cmd() in order to avoid transient drawing glitches.
			case APP_CMD_WINDOW_REDRAW_NEEDED:
				break;

			// Command from main thread: the content area of the window has changed, such as from the soft input window being shown or hidden. You can find the new content rect in android_app::contentRect.
			case APP_CMD_CONTENT_RECT_CHANGED:
				break;

			// Command from main thread: the app's activity window has gained input focus
			case APP_CMD_GAINED_FOCUS:
				Modules::Application->SendEvent(EAppEventType::EnterForeground);
				break;

			// Command from main thread: the app's activity window has lost input focus.
			case APP_CMD_LOST_FOCUS:
				Modules::Application->SendEvent(EAppEventType::EnterBackground);
				break;

			// Command from main thread: the current device configuration has changed.
			case APP_CMD_CONFIG_CHANGED:
				break;

			// Command from main thread: the system is running low on memory.
			case APP_CMD_LOW_MEMORY:
				Modules::Application->SendEvent(EAppEventType::LowMemory);
				break;

			// Command from main thread: the app's activity has been started.
			case APP_CMD_START:
				break;

			// Command from main thread: the app's activity has been resumed
			case APP_CMD_RESUME:
				break;

			// Command from main thread: the app should generate a new saved state for itself, to restore from later if needed.
			// If you have saved state, allocate it with malloc and place it in android_app.savedState with the size in android_app.savedStateSize. The memory will be freed for you later.
			case APP_CMD_SAVE_STATE:
				break;

			// Command from main thread: the app's activity has been paused.
			case APP_CMD_PAUSE:
				break;

			// Command from main thread: the app's activity has been stopped.
			case APP_CMD_STOP:
				break;

			// Command from main thread: the app's activity is being destroyed, and waiting for the app thread to clean up and exit before proceeding
			case APP_CMD_DESTROY:
				break;
		}
	}

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

#endif	// #if defined(DESIRE_PLATFORM_ANDROID)
