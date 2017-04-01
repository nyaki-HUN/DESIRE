#include "stdafx.h"
#include "Input/Mouse.h"
#include "Input/Input.h"

Mouse::Mouse(void *handle)
	: handle(handle)
	, deltaPosX(0)
	, deltaPosY(0)
	, wheelDelta(0)
{
	for(int i = 0; i < NUM_BUTTONS; ++i)
	{
		buttons[i] = 0;
	}
}

bool Mouse::IsDown(EButton button) const
{
	ASSERT(button < DESIRE_ASIZEOF(buttons));
	return (buttons[button] & Input::BUTTON_STATE_DOWN_FLAG) != 0;
}

bool Mouse::WentDown(EButton button) const
{
	return GetPressedCount(button) != 0;
}

uint8_t Mouse::GetPressedCount(EButton button) const
{
	ASSERT(button < DESIRE_ASIZEOF(buttons));
	return (uint8_t)(buttons[button] & ~Input::BUTTON_STATE_DOWN_FLAG);
}