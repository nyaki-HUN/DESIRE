#include "Engine/stdafx.h"
#include "Engine/Input/Mouse.h"

Mouse::Mouse(void* handle)
	: InputDevice(handle, offsetof(Mouse, buttons), NUM_BUTTONS, offsetof(Mouse, axes), NUM_AXES)
{
}
