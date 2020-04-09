#pragma once

#include "Engine/Input/InputDevice.h"

class Mouse : public InputDevice
{
	Mouse(void* handle);

public:
	enum EButton
	{
		Button_1,
		Button_Left = Button_1,
		Button_2,
		Button_Right = Button_2,
		Button_3,
		Button_Middle = Button_3,
		Button_4,
		Button_5,
		Num_Buttons
	};

	enum EAxis
	{
		Axis_X,
		Axis_Y,
		Wheel,
		Wheel_Horizontal,
		Num_Axis
	};

private:
	uint8_t buttons[Num_Buttons] = {};
	AxisState axes[Num_Axis] = {};

	friend class Input;
	friend class InputImpl;
};
