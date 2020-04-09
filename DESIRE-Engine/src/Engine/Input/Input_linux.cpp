#include "Engine/stdafx.h"
#include "Engine/Input/Input.h"

#if DESIRE_PLATFORM_LINUX

#include "Engine/Application/OSWindow.h"

#include <X11/Xlib.h>

constexpr int kFirstMappedKeyCode = 96;
constexpr int kLastMappedKeyCode = 126;

static Display* s_display = nullptr;
static EKeyCode s_keyConversionTable[kLastMappedKeyCode - kFirstMappedKeyCode + 1] = { (EKeyCode)0 };

// --------------------------------------------------------------------------------------------------------------------
//	InputImpl
// --------------------------------------------------------------------------------------------------------------------

class InputImpl
{
public:
	static void Handle_KeyPress_KeyRelease(const void* param)
	{
		const XEvent& event = *static_cast<const XEvent*>(param);
		Keyboard& keyboard = Modules::Input->GetKeyboardByHandle(nullptr);

		EKeyCode keyCode = (EKeyCode)0;
		if(event.xkey.keycode < kFirstMappedKeyCode)
		{
			keyCode = (EKeyCode)event.xkey.keycode;
		}
		else
		{
			int index = event.xkey.keycode - kFirstMappedKeyCode;
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

			char* typingCharacters = Modules::Input->typingCharacters;
			const size_t len = strlen(typingCharacters);
			const size_t bufferLen = strlen(buffer);
			if(len + bufferLen + 1 < Input::kMaxNumTypingCharacters)
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

	static void Handle_ButtonPress_ButtonRelease(const void* param)
	{
		const XEvent& event = *static_cast<const XEvent*>(param);
		Mouse& mouse = Modules::Input->GetMouseByHandle(nullptr);
		const bool isDown = (event.type == ButtonPress);

		switch(event.xbutton.button)
		{
			case Button1:	mouse.HandleButton(Mouse::Button_Left, isDown); break;
			case Button2:	mouse.HandleButton(Mouse::Button_Middle, isDown); break;
			case Button3:	mouse.HandleButton(Mouse::Button_Right, isDown); break;

			case Button4:
				if(isDown)
				{
					mouse.HandleAxis(Mouse::Wheel, 1.0f);
				}
				break;

			case Button5:
				if(isDown)
				{
					mouse.HandleAxis(Mouse::Wheel, -1.0f);
				}
				break;
		}
	}

	static void Handle_MotionNotify(const void* param)
	{
		const XEvent& event = *static_cast<const XEvent*>(param);
		Mouse& mouse = Modules::Input->GetMouseByHandle(nullptr);

		mouse.HandleAxisAbsolute(Mouse::Axis_X, (float)event.xmotion.x);
		mouse.HandleAxisAbsolute(Mouse::Axis_Y, (float)event.xmotion.y);

		Modules::Input->mouseCursorPos = Vector2((float)event.xmotion.x, (float)event.xmotion.y);
	}
};

// --------------------------------------------------------------------------------------------------------------------
//	Input
// --------------------------------------------------------------------------------------------------------------------

void Input::Init_internal(OSWindow* window)
{
	ASSERT(s_display == nullptr && "Input is already initialized");

	// Create local X Display connection
	s_display = XOpenDisplay(nullptr);
	if(s_display == nullptr)
	{
		LOG_ERROR("Unable to open a connection to the X server");
		return;
	}

	LINUXWindow* win = static_cast<LINUXWindow*>(window);
	win->RegisterMessageHandler(KeyPress, InputImpl::Handle_KeyPress_KeyRelease);
	win->RegisterMessageHandler(KeyRelease, InputImpl::Handle_KeyPress_KeyRelease);
	win->RegisterMessageHandler(ButtonPress, InputImpl::Handle_ButtonPress_ButtonRelease);
	win->RegisterMessageHandler(ButtonRelease, InputImpl::Handle_ButtonPress_ButtonRelease);
	win->RegisterMessageHandler(MotionNotify, InputImpl::Handle_MotionNotify);

	XSelectInput(s_display, (Window)window->GetHandle(), KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);

	// Add a default keyboard and mouse
	GetKeyboardByHandle(nullptr);
	GetMouseByHandle(nullptr);

	// Escaped scan code to EKeyCode mapping
	s_keyConversionTable[96 - kFirstMappedKeyCode] = KEY_NUMPADENTER;
	s_keyConversionTable[97 - kFirstMappedKeyCode] = KEY_RCONTROL;
	s_keyConversionTable[98 - kFirstMappedKeyCode] = KEY_NUMPADSLASH;
	s_keyConversionTable[100 - kFirstMappedKeyCode] = KEY_RALT;
	s_keyConversionTable[102 - kFirstMappedKeyCode] = KEY_HOME;
	s_keyConversionTable[103 - kFirstMappedKeyCode] = KEY_UP;
	s_keyConversionTable[104 - kFirstMappedKeyCode] = KEY_PGUP;
	s_keyConversionTable[105 - kFirstMappedKeyCode] = KEY_LEFT;
	s_keyConversionTable[106 - kFirstMappedKeyCode] = KEY_RIGHT;
	s_keyConversionTable[107 - kFirstMappedKeyCode] = KEY_END;
	s_keyConversionTable[108 - kFirstMappedKeyCode] = KEY_DOWN;
	s_keyConversionTable[109 - kFirstMappedKeyCode] = KEY_PGDOWN;
	s_keyConversionTable[110 - kFirstMappedKeyCode] = KEY_INSERT;
	s_keyConversionTable[111 - kFirstMappedKeyCode] = KEY_DELETE;
	s_keyConversionTable[125 - kFirstMappedKeyCode] = KEY_LWIN;
	s_keyConversionTable[126 - kFirstMappedKeyCode] = KEY_RWIN;
//	s_keyConversionTable[] = KEY_APPS;
}

void Input::Kill_internal()
{
	if(s_display != nullptr)
	{
		XCloseDisplay(s_display);
		s_display = nullptr;
	}
}

void Input::Update_internal()
{
}

#endif	// #if DESIRE_PLATFORM_LINUX
