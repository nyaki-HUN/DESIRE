#include "stdafx.h"
#include "Input/Mouse.h"
#include "Input/Input.h"

Mouse::Mouse(void *handle)
	: InputDevice(handle, offsetof(Mouse, buttons), NUM_BUTTONS, offsetof(Mouse, axes), NUM_AXES)
	, wheelDelta(0)
{
	memset(buttons, 0, sizeof(buttons));
	memset(axes, 0, sizeof(axes));
}
