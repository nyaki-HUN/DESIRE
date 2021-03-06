#include "Engine/stdafx.h"
#include "Engine/Input/Input.h"

#if DESIRE_PLATFORM_WINDOWS

#include "Engine/Application/OSWindow.h"

#include "Engine/Core/WINDOWS/os.h"

#define GET_MOUSE_X(lParam)	((int)(short)LOWORD(lParam))
#define GET_MOUSE_Y(lParam)	((int)(short)HIWORD(lParam))

// PageID and UsageIDs taken from http://www.usb.org/developers/devclass_docs/Hut1_12v2.pdf
#define HID_USAGE_PAGE_GENERIC		0x01
#define HID_USAGE_GENERIC_MOUSE		0x02
#define HID_USAGE_GENERIC_KEYBOARD	0x06

// --------------------------------------------------------------------------------------------------------------------
//	InputImpl
// --------------------------------------------------------------------------------------------------------------------

class InputImpl
{
public:
	static void Handle_WM_INPUT(const void* pParam)
	{
		WPARAM wParam = static_cast<const std::pair<WPARAM, LPARAM>*>(pParam)->first;
		LPARAM lParam = static_cast<const std::pair<WPARAM, LPARAM>*>(pParam)->second;

		if(GET_RAWINPUT_CODE_WPARAM(wParam) == RIM_INPUT)
		{
			RAWINPUT rawData;
			UINT dataSize = sizeof(rawData);
			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, &rawData, &dataSize, sizeof(RAWINPUTHEADER));

			if(rawData.header.dwType == RIM_TYPEMOUSE)
			{
				Mouse& mouse = Modules::Input->GetMouseByHandle(rawData.header.hDevice);

				if(rawData.data.mouse.usFlags == MOUSE_MOVE_RELATIVE)
				{
					// Mouse movement data is relative to the last mouse position
					mouse.HandleAxis(Mouse::Axis_X, (float)rawData.data.mouse.lLastX);
					mouse.HandleAxis(Mouse::Axis_Y, (float)rawData.data.mouse.lLastY);
				}
				else if(rawData.data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE)
				{
					// Mouse movement data is based on absolute position
					mouse.HandleAxisAbsolute(Mouse::Axis_X, (float)rawData.data.mouse.lLastX);
					mouse.HandleAxisAbsolute(Mouse::Axis_Y, (float)rawData.data.mouse.lLastY);
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
					mouse.HandleButton(Mouse::Button_Left, true);
				}
				else if(rawData.data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP)
				{
					mouse.HandleButton(Mouse::Button_Left, false);
				}

				// Right button
				if(rawData.data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN)
				{
					mouse.HandleButton(Mouse::Button_Right, true);
				}
				else if(rawData.data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP)
				{
					mouse.HandleButton(Mouse::Button_Right, false);
				}

				// Middle button
				if(rawData.data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN)
				{
					mouse.HandleButton(Mouse::Button_Middle, true);
				}
				else if(rawData.data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP)
				{
					mouse.HandleButton(Mouse::Button_Middle, false);
				}

				// Button 4
				if(rawData.data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_DOWN)
				{
					mouse.HandleButton(Mouse::Button_4, true);
				}
				else if(rawData.data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_UP)
				{
					mouse.HandleButton(Mouse::Button_4, false);
				}

				// Button 5
				if(rawData.data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_DOWN)
				{
					mouse.HandleButton(Mouse::Button_5, true);
				}
				else if(rawData.data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_UP)
				{
					mouse.HandleButton(Mouse::Button_5, false);
				}

				// Wheel and (vertical and horizontal)
				if(rawData.data.mouse.usButtonFlags & RI_MOUSE_WHEEL)
				{
					mouse.HandleAxis(Mouse::Wheel, (int16_t)rawData.data.mouse.usButtonData / (float)WHEEL_DELTA);
				}
				else if(rawData.data.mouse.usButtonFlags & RI_MOUSE_HWHEEL)
				{
					mouse.HandleAxis(Mouse::Wheel_Horizontal, (int16_t)rawData.data.mouse.usButtonData / (float)WHEEL_DELTA);
				}
			}
			else if(rawData.header.dwType == RIM_TYPEKEYBOARD)
			{
				Keyboard& keyboard = Modules::Input->GetKeyboardByHandle(rawData.header.hDevice);
				if(rawData.data.keyboard.Flags & RI_KEY_E1 || rawData.data.keyboard.VKey == 255)
				{
					// Discard "fake keys" which are part of an escaped sequence
					return;
				}

				uint8_t scanCode = (uint8_t)rawData.data.keyboard.MakeCode;
				if(rawData.data.keyboard.Flags & RI_KEY_E0)
				{
					scanCode |= kScanCode_E0_Mask;
				}

				keyboard.HandleButton(scanCode, !(rawData.data.keyboard.Flags & RI_KEY_BREAK));
			}
		}
	}

	static void Handle_WM_MOUSEMOVE(const void* pParam)
	{
		LPARAM lParam = static_cast<const std::pair<WPARAM, LPARAM>*>(pParam)->second;

		Modules::Input->m_mouseCursorPos = Vector2((float)GET_MOUSE_X(lParam), (float)GET_MOUSE_Y(lParam));
	}

	static void Handle_WM_CHAR(const void* pParam)
	{
		WPARAM wParam = static_cast<const std::pair<WPARAM, LPARAM>*>(pParam)->first;

		char ch = static_cast<char>(wParam);
		if(ch >= 0 && ch < 32)
		{
			// Discard control chars
			return;
		}

		Modules::Input->m_typingCharacters.AppendChar(ch);
	}
};

// --------------------------------------------------------------------------------------------------------------------
//	Input
// --------------------------------------------------------------------------------------------------------------------

void Input::Init_internal(OSWindow& window)
{
	window.RegisterMessageHandler(WM_INPUT, InputImpl::Handle_WM_INPUT);
	window.RegisterMessageHandler(WM_MOUSEMOVE, InputImpl::Handle_WM_MOUSEMOVE);
	window.RegisterMessageHandler(WM_CHAR, InputImpl::Handle_WM_CHAR);

	RAWINPUTDEVICE rawInputDevices[2];
	// Mouse
	rawInputDevices[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
	rawInputDevices[0].usUsage = HID_USAGE_GENERIC_MOUSE;
	rawInputDevices[0].dwFlags = 0;
	rawInputDevices[0].hwndTarget = static_cast<HWND>(window.GetHandle());
	// Keyboard
	rawInputDevices[1].usUsagePage = HID_USAGE_PAGE_GENERIC;
	rawInputDevices[1].usUsage = HID_USAGE_GENERIC_KEYBOARD;
	rawInputDevices[1].dwFlags = 0;
	rawInputDevices[1].hwndTarget = static_cast<HWND>(window.GetHandle());
	BOOL succeeded = RegisterRawInputDevices(rawInputDevices, 2, sizeof(RAWINPUTDEVICE));
	if(!succeeded)
	{
		LOG_ERROR_WITH_WIN32_ERRORCODE("RegisterRawInputDevices failed");
		return;
	}

	// Enumerate input devices
	UINT numDevices = 0;
	GetRawInputDeviceList(nullptr, &numDevices, sizeof(RAWINPUTDEVICELIST));
	std::unique_ptr<RAWINPUTDEVICELIST[]> deviceList = std::make_unique<RAWINPUTDEVICELIST[]>(numDevices);
	GetRawInputDeviceList(deviceList.get(), &numDevices, sizeof(RAWINPUTDEVICELIST));
	for(UINT i = 0; i < numDevices; ++i)
	{
		const RAWINPUTDEVICELIST& device = deviceList[i];
		if(device.dwType == RIM_TYPEKEYBOARD)
		{
			GetKeyboardByHandle(device.hDevice);
		}
		else if(device.dwType == RIM_TYPEMOUSE)
		{
			GetMouseByHandle(device.hDevice);
		}
	}
}

void Input::Kill_internal()
{
	RAWINPUTDEVICE rawInputDevices[2] = {};
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

void Input::Update_internal()
{
}

void Input::SetOsMouseCursorClipped(bool isClipped)
{
	if(m_isOsMouseCursorClipped == isClipped)
	{
		return;
	}

	if(isClipped)
	{
		POINT pos;
		GetCursorPos(&pos);

		const RECT clipRect = { pos.x, pos.y, clipRect.left + 1, clipRect.top + 1 };
		ClipCursor(&clipRect);
	}
	else
	{
		ClipCursor(nullptr);
	}

	m_isOsMouseCursorClipped = isClipped;
}

void Input::SetOsMouseCursorVisible(bool isVisible)
{
	if(m_isOsMouseCursorVisible == isVisible)
	{
		return;
	}

	ShowCursor(isVisible);
	m_isOsMouseCursorVisible = isVisible;
}

#endif	// #if DESIRE_PLATFORM_WINDOWS
