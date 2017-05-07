#pragma once

#include <X11/Xlib.h>

class InputImpl
{
public:
	static void Handle_KeyPress_KeyRelease(const XEvent& event);
	static void Handle_ButtonPress_ButtonRelease(const XEvent& event);
	static void Handle_MotionNotify(const XEvent& event);

	static Display *display;

	static const int FIRST_MAPPED_KEY_CODE = 96;
	static const int LAST_MAPPED_KEY_CODE = 126;
	static EKeyCode keyConversionTable[LAST_MAPPED_KEY_CODE - FIRST_MAPPED_KEY_CODE + 1];
};
