#include "stdafx.h"
#include "Input/Mouse.h"
#include "Input/Input.h"

Mouse::Mouse(void *handle)
	: InputDevice(handle, offsetof(Mouse, buttons), DESIRE_ASIZEOF(buttons), FIRST_MOUSE_BUTTON_ID)
	, deltaPosX(0)
	, deltaPosY(0)
	, wheelDelta(0)
{
	memset(buttons, 0, sizeof(buttons));
}
