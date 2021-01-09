#pragma once

#include "Engine/Input/InputDevice.h"
#include "Engine/Input/keyCodes.h"

class Keyboard : public InputDevice
{
	Keyboard(void* pHandle);

public:
	EKeyModifier GetActiveKeyModifier() const;

private:
	uint8_t m_keyStates[256] = {};

	friend class Input;
	friend class InputImpl;
};
