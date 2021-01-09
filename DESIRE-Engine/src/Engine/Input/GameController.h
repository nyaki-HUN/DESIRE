#pragma once

#include "Engine/Input/InputDevice.h"

class GameController : public InputDevice
{
	GameController(void* pHandle);

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
	uint8_t m_buttons[NUM_BUTTONS];
	AxisState m_axes[NUM_AXES];

	friend class Input;
	friend class InputImpl;
};
