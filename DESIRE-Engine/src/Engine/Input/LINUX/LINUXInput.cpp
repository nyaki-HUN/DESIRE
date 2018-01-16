#include "Engine/stdafx.h"
#include "Engine/Input/LINUX/LINUXInput.h"
#include "Engine/Input/Input.h"
#include "Engine/Input/Keyboard.h"
#include "Engine/Input/Mouse.h"
#include "Engine/Core/IWindow.h"

Display *InputImpl::display = nullptr;
EKeyCode InputImpl::keyConversionTable[InputImpl::LAST_MAPPED_KEY_CODE - InputImpl::FIRST_MAPPED_KEY_CODE + 1] = { (EKeyCode)0 };

void Input::Init(IWindow *window)
{
	ASSERT(InputImpl::display == nullptr && "Input is already initialized");

	// Create local X Display connection
	InputImpl::display = XOpenDisplay(nullptr);
	if(InputImpl::display == nullptr)
	{
		LOG_ERROR("Unable to open a connection to the X server");
		return;
	}

	LINUXWindow *win = static_cast<LINUXWindow*>(window);
	win->RegisterMessageHandler(KeyPress, InputImpl::Handle_KeyPress_KeyRelease);
	win->RegisterMessageHandler(KeyRelease, InputImpl::Handle_KeyPress_KeyRelease);
	win->RegisterMessageHandler(ButtonPress, InputImpl::Handle_ButtonPress_ButtonRelease);
	win->RegisterMessageHandler(ButtonRelease, InputImpl::Handle_ButtonPress_ButtonRelease);
	win->RegisterMessageHandler(MotionNotify, InputImpl::Handle_MotionNotify);

	XSelectInput(InputImpl::display, (Window)window->GetHandle(), KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);

	// Add a default keyboard and mouse
	if(Input::Get()->keyboards.empty())
	{
		Input::Get()->keyboards.push_back(Keyboard(nullptr));
	}
	if(Input::Get()->mouses.empty())
	{
		Input::Get()->mouses.push_back(Mouse(nullptr));
	}

	// Escaped scan code to EKeyCode mapping
	InputImpl::keyConversionTable[96 - InputImpl::FIRST_MAPPED_KEY_CODE] = KEY_NUMPADENTER;
	InputImpl::keyConversionTable[97 - InputImpl::FIRST_MAPPED_KEY_CODE] = KEY_RCONTROL;
	InputImpl::keyConversionTable[98 - InputImpl::FIRST_MAPPED_KEY_CODE] = KEY_NUMPADSLASH;
	InputImpl::keyConversionTable[100 - InputImpl::FIRST_MAPPED_KEY_CODE] = KEY_RALT;
	InputImpl::keyConversionTable[102 - InputImpl::FIRST_MAPPED_KEY_CODE] = KEY_HOME;
	InputImpl::keyConversionTable[103 - InputImpl::FIRST_MAPPED_KEY_CODE] = KEY_UP;
	InputImpl::keyConversionTable[104 - InputImpl::FIRST_MAPPED_KEY_CODE] = KEY_PGUP;
	InputImpl::keyConversionTable[105 - InputImpl::FIRST_MAPPED_KEY_CODE] = KEY_LEFT;
	InputImpl::keyConversionTable[106 - InputImpl::FIRST_MAPPED_KEY_CODE] = KEY_RIGHT;
	InputImpl::keyConversionTable[107 - InputImpl::FIRST_MAPPED_KEY_CODE] = KEY_END;
	InputImpl::keyConversionTable[108 - InputImpl::FIRST_MAPPED_KEY_CODE] = KEY_DOWN;
	InputImpl::keyConversionTable[109 - InputImpl::FIRST_MAPPED_KEY_CODE] = KEY_PGDOWN;
	InputImpl::keyConversionTable[110 - InputImpl::FIRST_MAPPED_KEY_CODE] = KEY_INSERT;
	InputImpl::keyConversionTable[111 - InputImpl::FIRST_MAPPED_KEY_CODE] = KEY_DELETE;
	InputImpl::keyConversionTable[125 - InputImpl::FIRST_MAPPED_KEY_CODE] = KEY_LWIN;
	InputImpl::keyConversionTable[126 - InputImpl::FIRST_MAPPED_KEY_CODE] = KEY_RWIN;
//	InputImpl::keyConversionTable[] = KEY_APPS;
}

void Input::Kill()
{
	if(InputImpl::display != nullptr)
	{
		XCloseDisplay(InputImpl::display);
		InputImpl::display = nullptr;
	}

	// Reset input devices
	Input::Get()->keyboards.clear();
	Input::Get()->mouses.clear();
	Input::Get()->gameControllers.clear();
}

void InputImpl::Handle_KeyPress_KeyRelease(const XEvent& event)
{
	ASSERT(!Input::Get()->keyboards.empty());
	Keyboard& keyboard = Input::Get()->keyboards.back();

	EKeyCode keyCode = (EKeyCode)0;
	if(event.xkey.keycode < FIRST_MAPPED_KEY_CODE)
	{
		keyCode = (EKeyCode)event.xkey.keycode;
	}
	else
	{
		int index = event.xkey.keycode - FIRST_MAPPED_KEY_CODE;
		if(index < DESIRE_ASIZEOF(keyConversionTable))
		{
			keyCode = keyConversionTable[index];
		}
	}

	if(event.type == KeyPress)
	{
		keyboard.HandleButton(keyCode, true);

		// Add typed UTF-8 character
		char buffer[5] = {};
		XLookupString(&event.xkey, buffer, DESIRE_ASIZEOF(buffer), nullptr, nullptr);

		char *typingCharacters = Input::Get()->typingCharacters;
		const size_t len = strlen(typingCharacters);
		const size_t bufferLen = strlen(buffer);
		if(len + bufferLen + 1 < Input::MAX_NUM_TYPING_CHARACTERS)
		{
			memcpy(&typingCharacters[len], buffer, bufferLen);
			typingCharacters[len + bufferLen + 1] = '\0';
		}
	}
	else
	{
		keyboard.HandleButton(keyCode, false);
	}
}

void InputImpl::Handle_ButtonPress_ButtonRelease(const XEvent& event)
{
	ASSERT(!Input::Get()->mouses.empty());
	Mouse& mouse = Input::Get()->mouses.back();

	const bool isDown = (event.type == ButtonPress)
	switch(event.xbutton.button)
	{
		case Button1:	mouse.HandleButton(Mouse::BUTTON_LEFT, isDown); break;
		case Button2:	mouse.HandleButton(Mouse::BUTTON_MIDDLE, isDown); break;
		case Button3:	mouse.HandleButton(Mouse::BUTTON_RIGHT, isDown); break;

		case Button4:
			if(isDown)
			{
				mouse.HandleAxis(Mouse::WHEEL, 1.0f);
			}
			break;

		case Button5:
			if(isDown)
			{
				mouse.HandleAxis(Mouse::WHEEL, -1.0f);
			}
			break;
	}
}

void InputImpl::Handle_MotionNotify(const XEvent& event)
{
	ASSERT(!Input::Get()->mouses.empty());
	Mouse& mouse = Input::Get()->mouses.back();

	mouse.HandleAxisAbsolute(Mouse::MOUSE_X, (float)event.xmotion.x);
	mouse.HandleAxisAbsolute(Mouse::MOUSE_Y, (float)event.xmotion.y);

	SPoint<int16_t>& mouseCursorPos = Input::Get()->mouseCursorPos;
	mouseCursorPos.Set((int16_t)event.xmotion.x, (int16_t)event.xmotion.y);
}
