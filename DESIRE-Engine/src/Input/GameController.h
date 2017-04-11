#pragma once

#include "Input/InputDevice.h"

class GameController : public InputDevice
{
	friend class Input;
	friend class InputImpl;

	GameController(void *handle);

public:
	enum EGameControllerDeviceButtonId
	{
		BTN_UP = FIRST_GAME_CONTROLLER_BUTTON_ID,
		BTN_RIGHT,
		BTN_DOWN,
		BTN_LEFT,

		BTN_X,

		LAST_GAME_CONTROLLER_BUTTON_ID,
	};

private:
	uint8_t buttons[LAST_GAME_CONTROLLER_BUTTON_ID - FIRST_GAME_CONTROLLER_BUTTON_ID];
};
