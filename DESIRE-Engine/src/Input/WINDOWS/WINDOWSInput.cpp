#include "stdafx.h"
#include "Input/WINDOWS/WINDOWSInput.h"
#include "Input/Input.h"
#include "Input/Keyboard.h"
#include "Input/Mouse.h"
#include "Core/WINDOWS/WINDOWSWindow.h"

#define GET_MOUSE_X(lParam)	((int)(short)LOWORD(lParam))
#define GET_MOUSE_Y(lParam)	((int)(short)HIWORD(lParam))

// PageID and UsageIDs taken from http://www.usb.org/developers/devclass_docs/Hut1_12v2.pdf
#define HID_USAGE_PAGE_GENERIC		0x01
#define HID_USAGE_GENERIC_MOUSE		0x02
#define HID_USAGE_GENERIC_KEYBOARD	0x06

void Input::Init(IWindow *window)
{
	WINDOWSWindow *win = static_cast<WINDOWSWindow*>(window);
	win->RegisterMessageHandler(WM_INPUT, InputImpl::Handle_WM_INPUT);
	win->RegisterMessageHandler(WM_MOUSEMOVE, InputImpl::Handle_WM_MOUSEMOVE);
	win->RegisterMessageHandler(WM_CHAR, InputImpl::Handle_WM_CHAR);

	RAWINPUTDEVICE rawInputDevices[2];
	// Mouse
	rawInputDevices[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
	rawInputDevices[0].usUsage = HID_USAGE_GENERIC_MOUSE;
	rawInputDevices[0].dwFlags = 0;
	rawInputDevices[0].hwndTarget = (HWND)window->GetHandle();
	// Keyboard
	rawInputDevices[1].usUsagePage = HID_USAGE_PAGE_GENERIC;
	rawInputDevices[1].usUsage = HID_USAGE_GENERIC_KEYBOARD;
	rawInputDevices[1].dwFlags = 0;
	rawInputDevices[1].hwndTarget = (HWND)window->GetHandle();
	BOOL succeeded = RegisterRawInputDevices(rawInputDevices, 2, sizeof(RAWINPUTDEVICE));
	if(!succeeded)
	{
		LOG_ERROR_WITH_WIN32_ERRORCODE("RegisterRawInputDevices failed");
		return;
	}

	// Enumerate input devices
	UINT numDevices = 0;
	GetRawInputDeviceList(nullptr, &numDevices, sizeof(RAWINPUTDEVICELIST));
	RAWINPUTDEVICELIST *deviceList = new RAWINPUTDEVICELIST[numDevices];
	GetRawInputDeviceList(deviceList, &numDevices, sizeof(RAWINPUTDEVICELIST));
	for(UINT i = 0; i < numDevices; ++i)
	{
		const RAWINPUTDEVICELIST& device = deviceList[i];
		if(device.dwType == RIM_TYPEKEYBOARD)
		{
			InputImpl::GetKeyboardByHandle(device.hDevice);
		}
		else if(device.dwType == RIM_TYPEMOUSE)
		{
			InputImpl::GetMouseByHandle(device.hDevice);
		}
	}
	delete[] deviceList;
}

void Input::Kill()
{
	RAWINPUTDEVICE rawInputDevices[2];
	// Mouse
	rawInputDevices[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
	rawInputDevices[0].usUsage = HID_USAGE_GENERIC_MOUSE;
	rawInputDevices[0].dwFlags = RIDEV_REMOVE;
	rawInputDevices[0].hwndTarget = nullptr;
	// Keyboard
	rawInputDevices[1].usUsagePage = HID_USAGE_PAGE_GENERIC;
	rawInputDevices[1].usUsage = HID_USAGE_GENERIC_KEYBOARD;
	rawInputDevices[1].dwFlags = RIDEV_REMOVE;
	rawInputDevices[1].hwndTarget = nullptr;
	RegisterRawInputDevices(rawInputDevices, 2, sizeof(RAWINPUTDEVICE));
}

void InputImpl::Handle_WM_INPUT(WPARAM wParam, LPARAM lParam)
{
	if(GET_RAWINPUT_CODE_WPARAM(wParam) == RIM_INPUT)
	{
		RAWINPUT rawData;
		UINT dataSize = sizeof(rawData);
		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, &rawData, &dataSize, sizeof(RAWINPUTHEADER));

		if(rawData.header.dwType == RIM_TYPEMOUSE)
		{
			Mouse& mouse = GetMouseByHandle(rawData.header.hDevice);
			if(rawData.data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE)
			{
				// Mouse movement data is based on absolute position
			}
			else
			{
				// Mouse movement data is relative to the last mouse position
				mouse.deltaPosX += (int16_t)rawData.data.mouse.lLastX;
				mouse.deltaPosY += (int16_t)rawData.data.mouse.lLastY;
			}
			if(rawData.data.mouse.usFlags & MOUSE_VIRTUAL_DESKTOP)
			{
				// Mouse coordinates are mapped to the virtual desktop (for a multiple monitor system)
			}
			if(rawData.data.mouse.usFlags & MOUSE_ATTRIBUTES_CHANGED)
			{
				// Mouse attributes changed, application needs to query the mouse attributes
			}

			// Left button
			if(rawData.data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN)
			{
				mouse.buttons[Mouse::BUTTON_LEFT]++;
				mouse.buttons[Mouse::BUTTON_LEFT] |= Input::BUTTON_STATE_DOWN_FLAG;
			}
			else if(rawData.data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP)
			{
				mouse.buttons[Mouse::BUTTON_LEFT] &= ~Input::BUTTON_STATE_DOWN_FLAG;
			}
			// Right button
			if(rawData.data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN)
			{
				mouse.buttons[Mouse::BUTTON_RIGHT]++;
				mouse.buttons[Mouse::BUTTON_RIGHT] |= Input::BUTTON_STATE_DOWN_FLAG;
			}
			else if(rawData.data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP)
			{
				mouse.buttons[Mouse::BUTTON_RIGHT] &= ~Input::BUTTON_STATE_DOWN_FLAG;
			}
			// Middle button
			if(rawData.data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN)
			{
				mouse.buttons[Mouse::BUTTON_MIDDLE]++;
				mouse.buttons[Mouse::BUTTON_MIDDLE] |= Input::BUTTON_STATE_DOWN_FLAG;
			}
			else if(rawData.data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP)
			{
				mouse.buttons[Mouse::BUTTON_MIDDLE] &= ~Input::BUTTON_STATE_DOWN_FLAG;
			}
			// Button 4
			if(rawData.data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_DOWN)
			{
				mouse.buttons[Mouse::BUTTON_4]++;
				mouse.buttons[Mouse::BUTTON_4] |= Input::BUTTON_STATE_DOWN_FLAG;
			}
			else if(rawData.data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_UP)
			{
				mouse.buttons[Mouse::BUTTON_4] &= ~Input::BUTTON_STATE_DOWN_FLAG;
			}
			// Button 5
			if(rawData.data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_DOWN)
			{
				mouse.buttons[Mouse::BUTTON_5]++;
				mouse.buttons[Mouse::BUTTON_5] |= Input::BUTTON_STATE_DOWN_FLAG;
			}
			else if(rawData.data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_UP)
			{
				mouse.buttons[Mouse::BUTTON_5] &= ~Input::BUTTON_STATE_DOWN_FLAG;
			}
			// Wheel
			if(rawData.data.mouse.usButtonFlags & RI_MOUSE_WHEEL)
			{
				mouse.wheelDelta += (int16_t)rawData.data.mouse.usButtonData / WHEEL_DELTA;
			}
		}
		else if(rawData.header.dwType == RIM_TYPEKEYBOARD)
		{
			Keyboard& keyboard = GetKeyboardByHandle(rawData.header.hDevice);
			if(rawData.data.keyboard.Flags & RI_KEY_E1 || rawData.data.keyboard.VKey == 255)
			{
				// Discard "fake keys" which are part of an escaped sequence
				return;
			}

			uint8_t scanCode = (uint8_t)rawData.data.keyboard.MakeCode;
			if(rawData.data.keyboard.Flags & RI_KEY_E0)
			{
				scanCode |= SCANCODE_E0_MASK;
			}

			if(rawData.data.keyboard.Flags & RI_KEY_BREAK)
			{
				keyboard.keyStates[scanCode] &= ~Input::BUTTON_STATE_DOWN_FLAG;
			}
			else
			{
				keyboard.keyStates[scanCode]++;
				keyboard.keyStates[scanCode] |= Input::BUTTON_STATE_DOWN_FLAG;
			}
		}
	}
}

void InputImpl::Handle_WM_MOUSEMOVE(WPARAM /*wParam*/, LPARAM lParam)
{
	Input::Get()->mouseCursorPos.Set(GET_MOUSE_X(lParam), GET_MOUSE_Y(lParam));
}

void InputImpl::Handle_WM_CHAR(WPARAM wParam, LPARAM /*lParam*/)
{
	char ch = (char)wParam;
	if(ch >= 0 && ch < 32)
	{
		// Discard control chars
		return;
	}

	char *typingCharacters = Input::Get()->typingCharacters;
	size_t len = strlen(typingCharacters);
	if(len + 1 < Input::MAX_NUM_TYPING_CHARACTERS)
	{
		typingCharacters[len] = ch;
		typingCharacters[len + 1] = '\0';
	}
}

Keyboard& InputImpl::GetKeyboardByHandle(HANDLE handle)
{
	std::vector<Keyboard>& keyboards = Input::Get()->keyboards;
	for(Keyboard& keyboard : keyboards)
	{
		if(keyboard.handle == handle)
		{
			return keyboard;
		}
	}

	// New keyboard found
	keyboards.push_back(Keyboard(handle));
	return keyboards.back();
}

Mouse& InputImpl::GetMouseByHandle(HANDLE handle)
{
	std::vector<Mouse>& mouses = Input::Get()->mouses;
	for(Mouse& mouse : mouses)
	{
		if(mouse.handle == handle)
		{
			return mouse;
		}
	}

	// New mouse found
	mouses.push_back(Mouse(handle));
	return mouses.back();
}
