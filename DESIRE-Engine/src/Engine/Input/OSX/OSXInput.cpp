#include "Engine/stdafx.h"
#include "Engine/Input/OSX/OSXInput.h"
#include "Engine/Input/Input.h"
#include "Engine/Input/Keyboard.h"
#include "Engine/Input/Mouse.h"

EventHandlerRef InputImpl::keyboardEventRef = nullptr;
EventHandlerRef InputImpl::mouseEventRef = nullptr;
EventHandlerUPP InputImpl::keyboardUPP = nullptr;
EventHandlerUPP InputImpl::mouseUPP = nullptr;
EKeyCode InputImpl::keyConversionTable[256] = {};

void Input::Init(IWindow *window)
{
	OSStatus status = noErr;
	EventTargetRef eventTargetRef = GetApplicationEventTarget();

	// Keyboard
	InputImpl::keyboardUPP = NewEventHandlerUPP(OSXKeyboardEventHandler);

	const EventTypeSpec keyEvents[] =
	{
		{ kEventClassKeyboard, kEventRawKeyDown },
		{ kEventClassKeyboard, kEventRawKeyUp },
		{ kEventClassKeyboard, kEventRawKeyModifiersChanged }
	};

	status = InstallEventHandler(eventTargetRef, InputImpl::keyboardUPP, GetEventTypeCount(keyEvents), keyEvents, nullptr, &InputImpl::keyboardEventRef);
	ASSERT(status == noErr);

	// Mouse
	InputImpl::mouseUPP = NewEventHandlerUPP(OSXMouseEventHandler);

	const EventTypeSpec mouseEvents[] =
	{
		{ kEventClassMouse, kEventMouseDown },
		{ kEventClassMouse, kEventMouseUp },
		{ kEventClassMouse, kEventMouseMoved },
		{ kEventClassMouse, kEventMouseDragged },
		{ kEventClassMouse, kEventMouseWheelMoved }
	};

	status = InstallEventHandler(eventTargetRef, InputImpl::mouseUPP, GetEventTypeCount(mouseEvents), mouseEvents, nullptr, &InputImpl::mouseEventRef);
	ASSERT(status == noErr);

	// Add a default keyboard and mouse
	if(Input::Get()->keyboards.empty())
	{
		Input::Get()->keyboards.push_back(Keyboard(nullptr));
	}
	if(Input::Get()->mouses.empty())
	{
		Input::Get()->mouses.push_back(Mouse(nullptr));
	}

	// Virtual Key to EKeyCode mapping
	InputImpl::keyConversionTable[53] = KEY_ESCAPE;
	InputImpl::keyConversionTable[18] = KEY_1;
	InputImpl::keyConversionTable[19] = KEY_2;
	InputImpl::keyConversionTable[20] = KEY_3;
	InputImpl::keyConversionTable[21] = KEY_4;
	InputImpl::keyConversionTable[23] = KEY_5;
	InputImpl::keyConversionTable[22] = KEY_6;
	InputImpl::keyConversionTable[26] = KEY_7;
	InputImpl::keyConversionTable[28] = KEY_8;
	InputImpl::keyConversionTable[25] = KEY_9;
	InputImpl::keyConversionTable[29] = KEY_0;
	InputImpl::keyConversionTable[27] = KEY_MINUS;
	InputImpl::keyConversionTable[24] = KEY_EQUALS;
	InputImpl::keyConversionTable[51] = KEY_BACKSPACE;
	InputImpl::keyConversionTable[48] = KEY_TAB;
	InputImpl::keyConversionTable[12] = KEY_Q;
	InputImpl::keyConversionTable[13] = KEY_W;
	InputImpl::keyConversionTable[14] = KEY_E;
	InputImpl::keyConversionTable[15] = KEY_R;
	InputImpl::keyConversionTable[17] = KEY_T;
	InputImpl::keyConversionTable[16] = KEY_Y;
	InputImpl::keyConversionTable[32] = KEY_U;
	InputImpl::keyConversionTable[34] = KEY_I;
	InputImpl::keyConversionTable[31] = KEY_O;
	InputImpl::keyConversionTable[35] = KEY_P;
	InputImpl::keyConversionTable[33] = KEY_LBRACKET;
	InputImpl::keyConversionTable[30] = KEY_RBRACKET;
	InputImpl::keyConversionTable[36] = KEY_RETURN;
	//InputImpl::keyConversionTable[] = KEY_LCONTROL;	// handled via kEventRawKeyModifiersChanged
	InputImpl::keyConversionTable[0] = KEY_A;
	InputImpl::keyConversionTable[1] = KEY_S;
	InputImpl::keyConversionTable[2] = KEY_D;
	InputImpl::keyConversionTable[3] = KEY_F;
	InputImpl::keyConversionTable[5] = KEY_G;
	InputImpl::keyConversionTable[4] = KEY_H;
	InputImpl::keyConversionTable[38] = KEY_J;
	InputImpl::keyConversionTable[40] = KEY_K;
	InputImpl::keyConversionTable[37] = KEY_L;
	InputImpl::keyConversionTable[41] = KEY_SEMICOLON;
	InputImpl::keyConversionTable[39] = KEY_APOSTROPHE;
	InputImpl::keyConversionTable[50] = KEY_GRAVE;
	//InputImpl::keyConversionTable[] = KEY_LSHIFT;		// handled via kEventRawKeyModifiersChanged
	InputImpl::keyConversionTable[42] = KEY_BACKSLASH;
	InputImpl::keyConversionTable[6] = KEY_Z;
	InputImpl::keyConversionTable[7] = KEY_X;
	InputImpl::keyConversionTable[8] = KEY_C;
	InputImpl::keyConversionTable[9] = KEY_V;
	InputImpl::keyConversionTable[11] = KEY_B;
	InputImpl::keyConversionTable[45] = KEY_N;
	InputImpl::keyConversionTable[46] = KEY_M;
	InputImpl::keyConversionTable[43] = KEY_COMMA;
	InputImpl::keyConversionTable[47] = KEY_PERIOD;
	InputImpl::keyConversionTable[44] = KEY_SLASH;
	//InputImpl::keyConversionTable[] = KEY_RSHIFT;		// handled via kEventRawKeyModifiersChanged
	InputImpl::keyConversionTable[67] = KEY_NUMPADSTAR;
	//InputImpl::keyConversionTable[] = KEY_LALT;		// handled via kEventRawKeyModifiersChanged
	InputImpl::keyConversionTable[49] = KEY_SPACE;
	InputImpl::keyConversionTable[57] = KEY_CAPSLOCK;
	InputImpl::keyConversionTable[122] = KEY_F1;
	InputImpl::keyConversionTable[120] = KEY_F2;
	InputImpl::keyConversionTable[99] = KEY_F3;
	InputImpl::keyConversionTable[118] = KEY_F4;
	InputImpl::keyConversionTable[96] = KEY_F5;
	InputImpl::keyConversionTable[97] = KEY_F6;
	InputImpl::keyConversionTable[98] = KEY_F7;
	InputImpl::keyConversionTable[100] = KEY_F8;
	InputImpl::keyConversionTable[101] = KEY_F9;
	InputImpl::keyConversionTable[109] = KEY_F10;
	InputImpl::keyConversionTable[71] = KEY_NUMLOCK;
//	InputImpl::keyConversionTable[] = KEY_SCROLL;
	InputImpl::keyConversionTable[89] = KEY_NUMPAD7;
	InputImpl::keyConversionTable[91] = KEY_NUMPAD8;
	InputImpl::keyConversionTable[92] = KEY_NUMPAD9;
	InputImpl::keyConversionTable[78] = KEY_NUMPADMINUS;
	InputImpl::keyConversionTable[86] = KEY_NUMPAD4;
	InputImpl::keyConversionTable[87] = KEY_NUMPAD5;
	InputImpl::keyConversionTable[88] = KEY_NUMPAD6;
	InputImpl::keyConversionTable[69] = KEY_NUMPADPLUS;
	InputImpl::keyConversionTable[83] = KEY_NUMPAD1;
	InputImpl::keyConversionTable[84] = KEY_NUMPAD2;
	InputImpl::keyConversionTable[85] = KEY_NUMPAD3;
	InputImpl::keyConversionTable[82] = KEY_NUMPAD0;
	InputImpl::keyConversionTable[65] = KEY_NUMPADPERIOD;
	InputImpl::keyConversionTable[10] = KEY_OEM_102;
	InputImpl::keyConversionTable[103] = KEY_F11;
	InputImpl::keyConversionTable[111] = KEY_F12;
	InputImpl::keyConversionTable[76] = KEY_NUMPADENTER;
	//InputImpl::keyConversionTable[] = KEY_RCONTROL;	// handled via kEventRawKeyModifiersChanged
	InputImpl::keyConversionTable[75] = KEY_NUMPADSLASH;
	//InputImpl::keyConversionTable[] = KEY_RALT;		// handled via kEventRawKeyModifiersChanged
	InputImpl::keyConversionTable[115] = KEY_HOME;
	InputImpl::keyConversionTable[126] = KEY_UP;
	InputImpl::keyConversionTable[116] = KEY_PGUP;
	InputImpl::keyConversionTable[123] = KEY_LEFT;
	InputImpl::keyConversionTable[124] = KEY_RIGHT;
	InputImpl::keyConversionTable[119] = KEY_END;
	InputImpl::keyConversionTable[125] = KEY_DOWN;
	InputImpl::keyConversionTable[121] = KEY_PGDOWN;
	InputImpl::keyConversionTable[114] = KEY_INSERT;
	InputImpl::keyConversionTable[117] = KEY_DELETE;
	//InputImpl::keyConversionTable[] = KEY_LWIN;		// handled via kEventRawKeyModifiersChanged
	//InputImpl::keyConversionTable[] = KEY_RWIN;		// handled via kEventRawKeyModifiersChanged
	//InputImpl::keyConversionTable[] = KEY_APPS;		// handled via kEventRawKeyModifiersChanged
}

void Input::Kill()
{
	if(InputImpl::keyboardEventRef != nullptr)
	{
		RemoveEventHandler(InputImpl::keyboardEventRef);
		InputImpl::keyboardEventRef = nullptr;
	}

	if(InputImpl::mouseEventRef != nullptr)
	{
		RemoveEventHandler(InputImpl::mouseEventRef);
		InputImpl::mouseEventRef = nullptr;
	}

	DisposeEventHandlerUPP(InputImpl::keyboardUPP);
	InputImpl::keyboardUPP = nullptr;
	DisposeEventHandlerUPP(InputImpl::mouseUPP);
	InputImpl::mouseUPP = nullptr;

	// Reset input devices
	Input::Get()->keyboards.clear();
	Input::Get()->mouses.clear();
	Input::Get()->gameControllers.clear();
}

OSStatus InputImpl::Handle_OSXKeyboardEvent(EventHandlerCallRef nextHandler, EventRef event, void *userData)
{
	ASSERT(!Input::Get()->keyboards.empty());
	Keyboard& keyboard = Input::Get()->keyboards.back();

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
//			keyboard.HandleButton(KEY_,			(modifiers & kEventKeyModifierNumLockMask));
			break;
		}
	}

	return CallNextEventHandler(nextHandler, event);
}

OSStatus InputImpl::Handle_OSXMouseEvent(EventHandlerCallRef nextHandler, EventRef event, void *userData)
{
	ASSERT(!Input::Get()->mouses.empty());
	Mouse& mouse = Input::Get()->mouses.back();

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
				mouse.HandleButton(Mouse::BUTTON_MIDDLE, true);
			}
			else if(button == kEventMouseButtonSecondary || (button == kEventMouseButtonPrimary && (modifiers & controlKey)))
			{
				mouse.HandleButton(Mouse::BUTTON_RIGHT, true);
			}
			else if(button == kEventMouseButtonPrimary)
			{
				mouse.HandleButton(Mouse::BUTTON_LEFT, true);
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
				mouse.HandleButton(Mouse::BUTTON_MIDDLE, false);
			}
			else if(button == kEventMouseButtonSecondary || (button == kEventMouseButtonPrimary && (modifiers & controlKey)))
			{
				mouse.HandleButton(Mouse::BUTTON_RIGHT, false);
			}
			else if(button == kEventMouseButtonPrimary)
			{
				mouse.HandleButton(Mouse::BUTTON_LEFT, false);
			}
			break;
		}

		case kEventMouseDragged:
		case kEventMouseMoved:
		{
			HIPoint delta = { 0.0f, 0.0f };
			GetEventParameter(event, kEventParamMouseDelta, typeHIPoint, nullptr, sizeof(HIPoint), nullptr, &delta);
			mouse.HandleAxis(Mouse::MOUSE_X, delta.x);
			mouse.HandleAxis(Mouse::MOUSE_Y, delta.y);

			HIPoint location = { 0.0f, 0.0f };
			GetEventParameter(event, kEventParamMouseLocation, typeHIPoint, nullptr, sizeof(HIPoint), nullptr, &location);
			Input::Get()->mouseCursorPos = Vector2(location.x, location.y);
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
				mouse.HandleAxis(Mouse::WHEEL, (float)wheelDelta);
			}
			else if(wheelAxis == kEventMouseWheelAxisX)
			{
				mouse.HandleAxis(Mouse::WHEEL_HORIZONTAL, (float)wheelDelta);
			}
			break;
		}
	}

	return CallNextEventHandler(nextHandler, event);
}
