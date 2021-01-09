#include "Engine/stdafx.h"
#include "Engine/Input/GameController.h"

#include "Engine/Input/Input.h"

GameController::GameController(void* pHandle)
	: InputDevice(pHandle, offsetof(GameController, m_buttons), NUM_BUTTONS, offsetof(GameController, m_axes), NUM_AXES)
{
}
