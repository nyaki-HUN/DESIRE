#pragma once

#include "Core/WINDOWS/os.h"

class Keyboard;
class Mouse;

class InputImpl
{
public:
	static void Handle_WM_INPUT(WPARAM wParam, LPARAM lParam);
	static void Handle_WM_MOUSEMOVE(WPARAM wParam, LPARAM lParam);
	static void Handle_WM_CHAR(WPARAM wParam, LPARAM lParam);

	static Keyboard& GetKeyboardByHandle(HANDLE handle);
	static Mouse& GetMouseByHandle(HANDLE handle);
};
