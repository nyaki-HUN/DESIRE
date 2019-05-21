#include "Engine/stdafx.h"

#if defined(DESIRE_PLATFORM_ANDROID)

#include "Engine/Application/Application.h"
#include "Engine/Application/EAppEventType.h"
#include "Engine/Application/OSWindow.h"

#include <android_native_app_glue.h>

static void Desire_HandleOnAppCmd(android_app *androidApp, int32_t cmd)
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
				OSWindow *window = reinterpret_cast<OSWindow*>(androidApp->userData);
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

static int32_t Desire_AndroidOnInputEvent(android_app *app, AInputEvent *event)
{
	if(AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
	{
		for(size_t i = 0; i < AMotionEvent_getPointerCount(event); ++i)
		{
			int32_t id = AMotionEvent_getPointerId(event, i);
			float x = AMotionEvent_getX(event, i);
			float y = AMotionEvent_getY(event, i);

			int32_t action = AMotionEvent_getAction(event);
			switch(action & AMOTION_EVENT_ACTION_MASK)
			{
				case AMOTION_EVENT_ACTION_UP:
					break;

				case AMOTION_EVENT_ACTION_DOWN:
					break;
			}
		}
		return 1;
	}

	return 0;
}

// Android native glue entry point
void android_main(android_app *androidApp)
{
	if(androidApp == nullptr || androidApp->activity == nullptr)
	{
		return;
	}

	// Make sure glue isn't stripped
	app_dummy();

	androidApp->userData = nullptr;
	androidApp->onAppCmd = Desire_HandleOnAppCmd;
	androidApp->onInputEvent = Desire_AndroidOnInputEvent;

	Application::Start(0, nullptr);
}

#endif	// #if defined(DESIRE_PLATFORM_ANDROID)
