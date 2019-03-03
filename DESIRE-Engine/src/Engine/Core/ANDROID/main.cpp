#include "stdafx.h"
#include "Engine/Core/Application.h"
#include "Engine/Core/ANDROID/ANDROIDWindow.h"

#include <android_native_app_glue.h>

int32_t Desire_AndroidOnInputEvent(android_app *app, AInputEvent *event)
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
	androidApp->onAppCmd = ANDROIDWindow::HandleOnAppCmd;
	androidApp->onInputEvent = Desire_AndroidOnInputEvent;

	Application::Start(0, nullptr);
}
