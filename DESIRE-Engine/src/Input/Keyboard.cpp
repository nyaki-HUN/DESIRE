#include "stdafx.h"
#include "Input/Keyboard.h"
#include "Input/Input.h"

Keyboard::Keyboard(void *handle)
	: InputDevice(handle, offsetof(Keyboard, keyStates), (uint16_t)DESIRE_ASIZEOF(keyStates))
{
	memset(keyStates, 0, sizeof(keyStates));
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
