#include "stdafx.h"
#include "Input/GameController.h"
#include "Input/Input.h"

GameController::GameController(void *handle)
	: handle(handle)
{

}

bool GameController::IsDown(EButton button) const
{
	return false;
}
