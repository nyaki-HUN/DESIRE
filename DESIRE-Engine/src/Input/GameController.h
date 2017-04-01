#pragma once

class GameController
{
	friend class Input;
	friend class InputImpl;

	GameController(void *handle);

public:
	enum EButton
	{
		BUTTON_UP,
		BUTTON_RIGHT,
		BUTTON_DOWN,
		BUTTON_LEFT,
	};

	bool IsDown(EButton button) const;

private:
	void *handle;
};
