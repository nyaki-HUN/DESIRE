#include "Engine/stdafx.h"
#include "Engine/Input/GameController.h"
#include "Engine/Input/Input.h"

GameController::GameController(void *handle)
	: InputDevice(handle, offsetof(GameController, buttons), NUM_BUTTONS, offsetof(GameController, axes), NUM_AXES)
{
	memset(buttons, 0, sizeof(buttons));
	memset(axes, 0, sizeof(axes));
}
