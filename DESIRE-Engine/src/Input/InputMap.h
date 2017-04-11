#pragma once

#include "Input/Keyboard.h"
#include "Input/Mouse.h"
#include "Input/GameController.h"

#include <vector>

class InputMap
{
public:
	InputMap();

	void MapButton(int userActionId, const InputDevice& inputDevice, int buttonId);

	bool IsDown(int userActionId) const;
	bool WentDown(int userActionId) const;

	// Returns how many times the action was triggered since the last frame
	uint8_t GetPressedCount(int userActionId) const;

private:
	struct SMappedInput
	{
		const void *inputDeviceHandle;
		int buttonId;
	};

	struct SUserAction
	{
		int id;
		std::vector<SMappedInput> mappedInputs;

		SUserAction(int userActionId)
			: id(userActionId)
		{

		}
	};

/*	struct SMappedAxis
	{
		std::vector<Mouse::EAxis> mappedMouseAxis;
		std::vector<GameController::EAxis> mappedGamepadAxis;
	};
*/
	const SUserAction* GetUserActionById(int userActionId) const;
	SUserAction& GetOrCreateUserActionById(int userActionId);

	std::vector<SUserAction> userActions;
};
