#pragma once

class Mouse
{
	friend class Input;
	friend class InputImpl;

	Mouse(void *handle);

public:
	enum EButton
	{
		BUTTON_LEFT,
		BUTTON_RIGHT,
		BUTTON_MIDDLE,
		BUTTON_1 = BUTTON_LEFT,
		BUTTON_2 = BUTTON_RIGHT,
		BUTTON_3 = BUTTON_MIDDLE,
		BUTTON_4,
		BUTTON_5,
		NUM_BUTTONS
	};

	bool IsDown(EButton button) const;
	bool WentDown(EButton button) const;

	// Returns how many times the button went down since the last frame
	uint8_t GetPressedCount(EButton button) const;

	int16_t deltaPosX;
	int16_t deltaPosY;
	int16_t wheelDelta;

private:
	uint8_t buttons[EButton::NUM_BUTTONS];
	void *handle;
};
