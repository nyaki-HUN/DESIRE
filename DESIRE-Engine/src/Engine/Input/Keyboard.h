#pragma once

#include "Engine/Input/InputDevice.h"
#include "Engine/Input/keyCodes.h"

class Keyboard : public InputDevice
{
	Keyboard(void* handle);

public:
	// Returns a combination of modifiers from Input::EModifierType
	uint8_t GetModifierMask() const;

private:
	uint8_t keyStates[256] = {};

	friend class Input;
	friend class InputImpl;
};
