#pragma once

#include "Engine/Input/InputDevice.h"

class GameController : public InputDevice
{
	friend class Input;
	friend class InputImpl;

	GameController(void *handle);

public:
	enum EButtonId
	{
		BTN_UP,
		BTN_RIGHT,
		BTN_DOWN,
		BTN_LEFT,

		BTN_X,

		NUM_BUTTONS
	};

	enum EAxisId
	{
		AXIS_1,

		NUM_AXES
	};


private:
	uint8_t buttons[NUM_BUTTONS];
	AxisState axes[NUM_AXES];
};
