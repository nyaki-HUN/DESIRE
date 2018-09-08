#pragma once

#include "Engine/Core/WINDOWS/os.h"

class Keyboard;
class Mouse;

class InputImpl
{
public:
	static void Handle_WM_INPUT(WPARAM wParam, LPARAM lParam);
	static void Handle_WM_MOUSEMOVE(WPARAM wParam, LPARAM lParam);
	static void Handle_WM_CHAR(WPARAM wParam, LPARAM lParam);
};
