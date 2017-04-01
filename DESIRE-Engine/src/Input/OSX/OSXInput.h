#pragma once

#include <Carbon/Carbon.h>

class InputImpl
{
public:
	static OSStatus Handle_OSXKeyboardEvent(EventHandlerCallRef nextHandler, EventRef event, void *userData);
	static OSStatus Handle_OSXMouseEvent(EventHandlerCallRef nextHandler, EventRef event, void *userData);

	static EventHandlerRef keyboardEventRef;
	static EventHandlerRef mouseEventRef;
	static EventHandlerUPP keyboardUPP;
	static EventHandlerUPP mouseUPP;

	static EKeyCode keyConversionTable[256];
};
