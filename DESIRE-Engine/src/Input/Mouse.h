#pragma once

#include "Input/InputDevice.h"

class Mouse : public InputDevice
{
	friend class Input;
	friend class InputImpl;

	Mouse(void *handle);

public:
	enum EMouseDeviceButtonId
	{
		BUTTON_1 = FIRST_MOUSE_BUTTON_ID,
		BUTTON_2,
		BUTTON_3,
		BUTTON_4,
		BUTTON_5,

		LAST_MOUSE_BUTTON_ID,

		// Helpers
		BUTTON_LEFT = BUTTON_1,
		BUTTON_RIGHT = BUTTON_2,
		BUTTON_MIDDLE = BUTTON_3
	};

	int16_t deltaPosX;
	int16_t deltaPosY;
	int16_t wheelDelta;

private:
	uint8_t buttons[LAST_MOUSE_BUTTON_ID - FIRST_MOUSE_BUTTON_ID];
};
