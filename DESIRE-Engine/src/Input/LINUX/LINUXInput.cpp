#include "stdafx.h"
#include "Input/LINUX/LINUXInput.h"
#include "Input/Input.h"
#include "Input/Keyboard.h"
#include "Input/Mouse.h"
#include "Core/IWindow.h"

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

void InputImpl::HandleKeyboardAndMouseEvents(EventHandlerCallRef nextHandler, EventRef event, void *userData)
{
	SPoint<int16_t>& mouseCursorPos = Input::Get()->mouseCursorPos;

	ASSERT(!Input::Get()->keyboards.empty());
	ASSERT(!Input::Get()->mouses.empty());
	Keyboard& keyboard = Input::Get()->keyboards.back();
	Mouse& mouse = Input::Get()->mouses.back();

	XEvent event;
	while(XPending(display) > 0)
	{
		XNextEvent(display, &event);
		switch(event.type)
		{
			case KeyPress:
			{
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

				keyboard.SetButtonStateDown(keyCode, true);

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
				break;
			}

			case KeyRelease:
			{
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

				keyboard.SetButtonStateDown(keyCode, false);
				break;
			}

			case ButtonPress:
				switch(event.xbutton.button)
				{
					case Button1:	mouse.SetButtonStateDown(Mouse::BUTTON_LEFT, true); break;
					case Button2:	mouse.SetButtonStateDown(Mouse::BUTTON_MIDDLE, true); break;
					case Button3:	mouse.SetButtonStateDown(Mouse::BUTTON_RIGHT, true); break;
					case Button4:	mouse.wheelDelta += 120; break;
					case Button5:	mouse.wheelDelta -= 120; break;
				}
				break;

			case ButtonRelease:
				switch(event.xbutton.button)
				{
					case Button1:	mouse.SetButtonStateDown(Mouse::BUTTON_LEFT, false); break;
					case Button2:	mouse.SetButtonStateDown(Mouse::BUTTON_MIDDLE, false); break;
					case Button3:	mouse.SetButtonStateDown(Mouse::BUTTON_RIGHT, false); break;
				}
				break;

			case MotionNotify:
				mouse.deltaPosX += (int16_t)event.xmotion.x - mouseCursorPos.x;
				mouse.deltaPosY += (int16_t)event.xmotion.y - mouseCursorPos.y;
				mouseCursorPos.Set((int16_t)event.xmotion.x, (int16_t)event.xmotion.y);
				break;

			case MappingNotify:
				break;

			default:
				LOG_DEBUG("Unknown X Event: %d", event.type);
				break;
		}
	}
}
