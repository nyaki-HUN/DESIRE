#pragma once

class InputDevice
{
protected:
	InputDevice(void *handle, size_t offsetOfButtonStates, size_t numButtonStates, int firstButtonId);

public:
	static const uint8_t BUTTON_STATE_DOWN_FLAG = 0x80;

	enum EInputDeviceButtonId
	{
		FIRST_KEYBOARD_BUTTON_ID = 0,
		FIRST_MOUSE_BUTTON_ID = 0x100,
		FIRST_GAME_CONTROLLER_BUTTON_ID = 0x200
	};

	bool IsDown(int buttonId) const;
	bool WentDown(int buttonId) const;

	// Returns how many times the button went down since the last frame
	uint8_t GetPressedCount(int buttonId) const;

	void *handle;

protected:
	void SetButtonStateDown(int buttonId, bool isDown);

	const size_t offsetOfButtonStatesInDerivedClass;
	const size_t numButtonStates;
	const int firstButtonId;
};
