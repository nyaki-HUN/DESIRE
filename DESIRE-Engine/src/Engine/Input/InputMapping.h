#pragma once

#include "Engine/Core/Container/Array.h"

class InputDevice;

class InputMapping
{
public:
	InputMapping();

	void MapButton(int userActionId, const InputDevice& inputDevice, int buttonId);
	void MapAxis(int userActionId, const InputDevice& inputDevice, int axisId);
	bool IsMapped(int userActionId) const;

	// Removes all mappings for the given user action
	void Unmap(int userActionId);

	bool IsDown(int userActionId) const;
	bool WasPressed(int userActionId) const;

	// Returns how many times the action was triggered since the last frame
	uint8_t GetPressedCount(int userActionId) const;

	float GetFloatState(int userActionId) const;

private:
	struct MappedInput
	{
		int id;
		const void* inputDeviceHandle;
	};

	struct MappedAxis : public MappedInput
	{
		float deadZone;
	};

	struct UserAction
	{
		int id;
		Array<MappedInput> mappedButtons;
		Array<MappedAxis> mappedAxes;

		UserAction(int userActionId)
			: id(userActionId)
		{

		}
	};

	UserAction& GetOrAddUserActionById(int userActionId);

	Array<UserAction> userActions;
};
