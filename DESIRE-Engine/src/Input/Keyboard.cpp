#include "stdafx.h"
#include "Input/Keyboard.h"
#include "Input/Input.h"

Keyboard::Keyboard(void *handle)
	: handle(handle)
{
	for(int i = 0; i < 256; ++i)
	{
		keyStates[i] = 0;
	}
}

bool Keyboard::IsDown(EKeyCode keyCode) const
{
	ASSERT(keyCode < DESIRE_ASIZEOF(keyStates));
	return (keyStates[keyCode] & Input::BUTTON_STATE_DOWN_FLAG) != 0;
}

bool Keyboard::WentDown(EKeyCode keyCode) const
{
	return GetPressedCount(keyCode) != 0;
}

uint8_t Keyboard::GetPressedCount(EKeyCode keyCode) const
{
	ASSERT(keyCode < DESIRE_ASIZEOF(keyStates));
	return (uint8_t)(keyStates[keyCode] & ~Input::BUTTON_STATE_DOWN_FLAG);
}

uint8_t Keyboard::GetModifierMask() const
{
	uint8_t modifiers = 0;
	if(IsDown(KEY_LCONTROL) || IsDown(KEY_RCONTROL))
	{
		modifiers |= Input::MODIFIER_CTRL;
	}
	if(IsDown(KEY_LSHIFT) || IsDown(KEY_RSHIFT))
	{
		modifiers |= Input::MODIFIER_SHIFT;
	}
	if(IsDown(KEY_LALT) || IsDown(KEY_RALT))
	{
		modifiers |= Input::MODIFIER_ALT;
	}
	if(IsDown(KEY_LWIN) || IsDown(KEY_RWIN))
	{
		modifiers |= Input::MODIFIER_COMMAND;
	}
	return modifiers;
}
