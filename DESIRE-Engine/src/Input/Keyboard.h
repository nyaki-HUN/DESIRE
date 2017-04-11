#pragma once

#include "Input/InputDevice.h"
#include "Input/keyCodes.h"

class Keyboard : public InputDevice
{
	friend class Input;
	friend class InputImpl;

	Keyboard(void *handle);

public:
	// Returns a combination of modifiers from Input::EModifierType
	uint8_t GetModifierMask() const;

private:
	uint8_t keyStates[256];
};
