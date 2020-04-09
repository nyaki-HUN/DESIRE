#include "Engine/stdafx.h"
#include "Engine/Input/Input.h"

#if DESIRE_PLATFORM_OSX

#include <Carbon/Carbon.h>

static EventHandlerRef s_keyboardEventRef = nullptr;
static EventHandlerRef s_mouseEventRef = nullptr;
static EventHandlerUPP s_keyboardUPP = nullptr;
static EventHandlerUPP s_mouseUPP = nullptr;
static EKeyCode s_keyConversionTable[256] = {};

// --------------------------------------------------------------------------------------------------------------------
//	InputImpl
// --------------------------------------------------------------------------------------------------------------------

class InputImpl
{
public:
	static OSStatus Handle_OSXKeyboardEvent(EventHandlerCallRef nextHandler, EventRef event, void* userData)
	{
		Keyboard& keyboard = Modules::Input->GetKeyboardByHandle(nullptr);

		uint32 eventKind = GetEventKind(event);
		switch(eventKind)
		{
			case kEventRawKeyDown:
			{
				UInt32 virtualKey;
				GetEventParameter(event, 'kcod', typeUInt32, nullptr, sizeof(UInt32), nullptr, &virtualKey);
				if(virtualKey < DESIRE_ASIZEOF(keyConversionTable))
				{
					const EKeyCode keyCode = keyConversionTable[virtualKey];
					keyboard.HandleButton(keyCode, true);
				}
				break;
			}

			case kEventRawKeyUp:
			{
				UInt32 virtualKey;
				GetEventParameter(event, 'kcod', typeUInt32, nullptr, sizeof(UInt32), nullptr, &virtualKey);
				if(virtualKey < DESIRE_ASIZEOF(keyConversionTable))
				{
					const EKeyCode keyCode = keyConversionTable[virtualKey];
					keyboard.HandleButton(keyCode, false);
				}
				break;
			}

			case kEventRawKeyModifiersChanged:
			{
				UInt32 modifiers = 0;
				GetEventParameter(event, kEventParamKeyModifiers, typeUInt32, nullptr, sizeof(UInt32), nullptr, &modifiers);

				keyboard.HandleButton(KEY_LCONTROL,	(modifiers & controlKey));
				keyboard.HandleButton(KEY_LSHIFT,	(modifiers & shiftKey));
				keyboard.HandleButton(KEY_LALT,		(modifiers & optionKey));
				keyboard.HandleButton(KEY_LWIN,		(modifiers & cmdKey));
				keyboard.HandleButton(KEY_CAPSLOCK,	(modifiers & alphaLock));
				keyboard.HandleButton(KEY_APPS,		(modifiers & kEventKeyModifierFnMask));
//				keyboard.HandleButton(KEY_,			(modifiers & kEventKeyModifierNumLockMask));
				break;
			}
		}

		return CallNextEventHandler(nextHandler, event);
	}

	static OSStatus Handle_OSXMouseEvent(EventHandlerCallRef nextHandler, EventRef event, void* userData)
	{
		Mouse& mouse = Modules::Input->GetMouseByHandle(nullptr);

		UInt32 eventKind = GetEventKind(event);
		switch(eventKind)
		{
			case kEventMouseDown:
			{
				EventMouseButton button = 0;
				UInt32 modifiers = 0;
				GetEventParameter(event, kEventParamMouseButton, typeMouseButton, nullptr, sizeof(EventMouseButton), nullptr, &button);
				GetEventParameter(event, kEventParamKeyModifiers, typeUInt32, nullptr, sizeof(UInt32), nullptr, &modifiers);

				if(button == kEventMouseButtonTertiary || (button == kEventMouseButtonPrimary && (modifiers & optionKey)))
				{
					mouse.HandleButton(Mouse::Button_Middle, true);
				}
				else if(button == kEventMouseButtonSecondary || (button == kEventMouseButtonPrimary && (modifiers & controlKey)))
				{
					mouse.HandleButton(Mouse::Button_Right, true);
				}
				else if(button == kEventMouseButtonPrimary)
				{
					mouse.HandleButton(Mouse::Button_Left, true);
				}
				break;
			}

			case kEventMouseUp:
			{
				EventMouseButton button = 0;
				UInt32 modifiers = 0;
				GetEventParameter(event, kEventParamMouseButton, typeMouseButton, nullptr, sizeof(EventMouseButton), nullptr, &button);
				GetEventParameter(event, kEventParamKeyModifiers, typeUInt32, nullptr, sizeof(UInt32), nullptr, &modifiers);

				if(button == kEventMouseButtonTertiary || (button == kEventMouseButtonPrimary && (modifiers & optionKey)))
				{
					mouse.HandleButton(Mouse::Button_Middle, false);
				}
				else if(button == kEventMouseButtonSecondary || (button == kEventMouseButtonPrimary && (modifiers & controlKey)))
				{
					mouse.HandleButton(Mouse::Button_Right, false);
				}
				else if(button == kEventMouseButtonPrimary)
				{
					mouse.HandleButton(Mouse::Button_Left, false);
				}
				break;
			}

			case kEventMouseDragged:
			case kEventMouseMoved:
			{
				HIPoint delta = { 0.0f, 0.0f };
				GetEventParameter(event, kEventParamMouseDelta, typeHIPoint, nullptr, sizeof(HIPoint), nullptr, &delta);
				mouse.HandleAxis(Mouse::Axis_X, delta.x);
				mouse.HandleAxis(Mouse::Axis_Y, delta.y);

				HIPoint location = { 0.0f, 0.0f };
				GetEventParameter(event, kEventParamMouseLocation, typeHIPoint, nullptr, sizeof(HIPoint), nullptr, &location);
				Modules::Input->mouseCursorPos = Vector2(location.x, location.y);
				break;
			}

			case kEventMouseWheelMoved:
			{
				EventMouseWheelAxis wheelAxis = 0;
				GetEventParameter(event, kEventParamMouseWheelAxis, typeMouseWheelAxis, nullptr, sizeof(EventMouseWheelAxis), nullptr, &wheelAxis);
				SInt32 wheelDelta = 0;
				GetEventParameter(event, kEventParamMouseWheelDelta, typeSInt32, nullptr, sizeof(SInt32), nullptr, &wheelDelta);

				if(wheelAxis == kEventMouseWheelAxisY)
				{
					mouse.HandleAxis(Mouse::Wheel, (float)wheelDelta);
				}
				else if(wheelAxis == kEventMouseWheelAxisX)
				{
					mouse.HandleAxis(Mouse::Wheel_Horizontal, (float)wheelDelta);
				}
				break;
			}
		}

		return CallNextEventHandler(nextHandler, event);
	}
};

// --------------------------------------------------------------------------------------------------------------------
//	Input
// --------------------------------------------------------------------------------------------------------------------

void Input::Init_internal(OSWindow* window)
{
	OSStatus status = noErr;
	EventTargetRef eventTargetRef = GetApplicationEventTarget();

	// Keyboard
	s_keyboardUPP = NewEventHandlerUPP(OSXKeyboardEventHandler);

	const EventTypeSpec keyEvents[] =
	{
		{ kEventClassKeyboard, kEventRawKeyDown },
		{ kEventClassKeyboard, kEventRawKeyUp },
		{ kEventClassKeyboard, kEventRawKeyModifiersChanged }
	};

	status = InstallEventHandler(eventTargetRef, s_keyboardUPP, GetEventTypeCount(keyEvents), keyEvents, nullptr, &s_keyboardEventRef);
	ASSERT(status == noErr);

	// Mouse
	s_mouseUPP = NewEventHandlerUPP(OSXMouseEventHandler);

	const EventTypeSpec mouseEvents[] =
	{
		{ kEventClassMouse, kEventMouseDown },
		{ kEventClassMouse, kEventMouseUp },
		{ kEventClassMouse, kEventMouseMoved },
		{ kEventClassMouse, kEventMouseDragged },
		{ kEventClassMouse, kEventMouseWheelMoved }
	};

	status = InstallEventHandler(eventTargetRef, s_mouseUPP, GetEventTypeCount(mouseEvents), mouseEvents, nullptr, &s_mouseEventRef);
	ASSERT(status == noErr);

	// Add a default keyboard and mouse
	GetKeyboardByHandle(nullptr);
	GetMouseByHandle(nullptr);

	// Virtual Key to EKeyCode mapping
	s_keyConversionTable[53] = KEY_ESCAPE;
	s_keyConversionTable[18] = KEY_1;
	s_keyConversionTable[19] = KEY_2;
	s_keyConversionTable[20] = KEY_3;
	s_keyConversionTable[21] = KEY_4;
	s_keyConversionTable[23] = KEY_5;
	s_keyConversionTable[22] = KEY_6;
	s_keyConversionTable[26] = KEY_7;
	s_keyConversionTable[28] = KEY_8;
	s_keyConversionTable[25] = KEY_9;
	s_keyConversionTable[29] = KEY_0;
	s_keyConversionTable[27] = KEY_MINUS;
	s_keyConversionTable[24] = KEY_EQUALS;
	s_keyConversionTable[51] = KEY_BACKSPACE;
	s_keyConversionTable[48] = KEY_TAB;
	s_keyConversionTable[12] = KEY_Q;
	s_keyConversionTable[13] = KEY_W;
	s_keyConversionTable[14] = KEY_E;
	s_keyConversionTable[15] = KEY_R;
	s_keyConversionTable[17] = KEY_T;
	s_keyConversionTable[16] = KEY_Y;
	s_keyConversionTable[32] = KEY_U;
	s_keyConversionTable[34] = KEY_I;
	s_keyConversionTable[31] = KEY_O;
	s_keyConversionTable[35] = KEY_P;
	s_keyConversionTable[33] = KEY_LBRACKET;
	s_keyConversionTable[30] = KEY_RBRACKET;
	s_keyConversionTable[36] = KEY_RETURN;
	//s_keyConversionTable[] = KEY_LCONTROL;	// handled via kEventRawKeyModifiersChanged
	s_keyConversionTable[0] = KEY_A;
	s_keyConversionTable[1] = KEY_S;
	s_keyConversionTable[2] = KEY_D;
	s_keyConversionTable[3] = KEY_F;
	s_keyConversionTable[5] = KEY_G;
	s_keyConversionTable[4] = KEY_H;
	s_keyConversionTable[38] = KEY_J;
	s_keyConversionTable[40] = KEY_K;
	s_keyConversionTable[37] = KEY_L;
	s_keyConversionTable[41] = KEY_SEMICOLON;
	s_keyConversionTable[39] = KEY_APOSTROPHE;
	s_keyConversionTable[50] = KEY_GRAVE;
	//s_keyConversionTable[] = KEY_LSHIFT;		// handled via kEventRawKeyModifiersChanged
	s_keyConversionTable[42] = KEY_BACKSLASH;
	s_keyConversionTable[6] = KEY_Z;
	s_keyConversionTable[7] = KEY_X;
	s_keyConversionTable[8] = KEY_C;
	s_keyConversionTable[9] = KEY_V;
	s_keyConversionTable[11] = KEY_B;
	s_keyConversionTable[45] = KEY_N;
	s_keyConversionTable[46] = KEY_M;
	s_keyConversionTable[43] = KEY_COMMA;
	s_keyConversionTable[47] = KEY_PERIOD;
	s_keyConversionTable[44] = KEY_SLASH;
	//s_keyConversionTable[] = KEY_RSHIFT;		// handled via kEventRawKeyModifiersChanged
	s_keyConversionTable[67] = KEY_NUMPADSTAR;
	//s_keyConversionTable[] = KEY_LALT;		// handled via kEventRawKeyModifiersChanged
	s_keyConversionTable[49] = KEY_SPACE;
	s_keyConversionTable[57] = KEY_CAPSLOCK;
	s_keyConversionTable[122] = KEY_F1;
	s_keyConversionTable[120] = KEY_F2;
	s_keyConversionTable[99] = KEY_F3;
	s_keyConversionTable[118] = KEY_F4;
	s_keyConversionTable[96] = KEY_F5;
	s_keyConversionTable[97] = KEY_F6;
	s_keyConversionTable[98] = KEY_F7;
	s_keyConversionTable[100] = KEY_F8;
	s_keyConversionTable[101] = KEY_F9;
	s_keyConversionTable[109] = KEY_F10;
	s_keyConversionTable[71] = KEY_NUMLOCK;
//	s_keyConversionTable[] = KEY_SCROLL;
	s_keyConversionTable[89] = KEY_NUMPAD7;
	s_keyConversionTable[91] = KEY_NUMPAD8;
	s_keyConversionTable[92] = KEY_NUMPAD9;
	s_keyConversionTable[78] = KEY_NUMPADMINUS;
	s_keyConversionTable[86] = KEY_NUMPAD4;
	s_keyConversionTable[87] = KEY_NUMPAD5;
	s_keyConversionTable[88] = KEY_NUMPAD6;
	s_keyConversionTable[69] = KEY_NUMPADPLUS;
	s_keyConversionTable[83] = KEY_NUMPAD1;
	s_keyConversionTable[84] = KEY_NUMPAD2;
	s_keyConversionTable[85] = KEY_NUMPAD3;
	s_keyConversionTable[82] = KEY_NUMPAD0;
	s_keyConversionTable[65] = KEY_NUMPADPERIOD;
	s_keyConversionTable[10] = KEY_OEM_102;
	s_keyConversionTable[103] = KEY_F11;
	s_keyConversionTable[111] = KEY_F12;
	s_keyConversionTable[76] = KEY_NUMPADENTER;
	//s_keyConversionTable[] = KEY_RCONTROL;	// handled via kEventRawKeyModifiersChanged
	s_keyConversionTable[75] = KEY_NUMPADSLASH;
	//s_keyConversionTable[] = KEY_RALT;		// handled via kEventRawKeyModifiersChanged
	s_keyConversionTable[115] = KEY_HOME;
	s_keyConversionTable[126] = KEY_UP;
	s_keyConversionTable[116] = KEY_PGUP;
	s_keyConversionTable[123] = KEY_LEFT;
	s_keyConversionTable[124] = KEY_RIGHT;
	s_keyConversionTable[119] = KEY_END;
	s_keyConversionTable[125] = KEY_DOWN;
	s_keyConversionTable[121] = KEY_PGDOWN;
	s_keyConversionTable[114] = KEY_INSERT;
	s_keyConversionTable[117] = KEY_DELETE;
	//s_keyConversionTable[] = KEY_LWIN;		// handled via kEventRawKeyModifiersChanged
	//s_keyConversionTable[] = KEY_RWIN;		// handled via kEventRawKeyModifiersChanged
	//s_keyConversionTable[] = KEY_APPS;		// handled via kEventRawKeyModifiersChanged
}

void Input::Kill_internal()
{
	if(s_keyboardEventRef != nullptr)
	{
		RemoveEventHandler(s_keyboardEventRef);
		s_keyboardEventRef = nullptr;
	}

	if(s_mouseEventRef != nullptr)
	{
		RemoveEventHandler(s_mouseEventRef);
		s_mouseEventRef = nullptr;
	}

	DisposeEventHandlerUPP(s_keyboardUPP);
	s_keyboardUPP = nullptr;
	DisposeEventHandlerUPP(s_mouseUPP);
	s_mouseUPP = nullptr;
}

void Input::Update_internal()
{
}

#endif	// #if DESIRE_PLATFORM_OSX
