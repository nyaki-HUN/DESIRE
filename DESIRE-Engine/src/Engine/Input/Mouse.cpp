#include "Engine/stdafx.h"
#include "Engine/Input/Mouse.h"

Mouse::Mouse(void* handle)
	: InputDevice(handle, offsetof(Mouse, buttons), static_cast<uint16_t>(DESIRE_ASIZEOF(buttons)), offsetof(Mouse, axes), static_cast<uint16_t>(DESIRE_ASIZEOF(axes)))
{
}
