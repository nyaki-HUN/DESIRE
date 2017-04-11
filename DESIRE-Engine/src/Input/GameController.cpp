#include "stdafx.h"
#include "Input/GameController.h"
#include "Input/Input.h"

GameController::GameController(void *handle)
	: InputDevice(handle, offsetof(GameController, buttons), DESIRE_ASIZEOF(buttons), FIRST_GAME_CONTROLLER_BUTTON_ID)
{
	memset(buttons, 0, sizeof(buttons));
}
