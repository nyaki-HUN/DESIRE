#pragma once

#include "Input/keyCodes.h"

class Keyboard
{
	friend class Input;
	friend class InputImpl;

	Keyboard(void *handle);

public:
	bool IsDown(EKeyCode keyCode) const;
	bool WentDown(EKeyCode keyCode) const;

	// Returns how many times the key went down since the last frame
	uint8_t GetPressedCount(EKeyCode keyCode) const;

	// Returns a combination of modifiers from Input::EModifierType
	uint8_t GetModifierMask() const;

private:
	uint8_t keyStates[256];
	void *handle;
};
