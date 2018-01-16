#pragma once

#include <vector>

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
	bool WentDown(int userActionId) const;

	// Returns how many times the action was triggered since the last frame
	uint8_t GetPressedCount(int userActionId) const;

	float GetFloatState(int userActionId) const;

private:
	struct MappedInput
	{
		int id;
		const void *inputDeviceHandle;
	};

	struct MappedAxis : public MappedInput
	{
		float deadZone;
	};

	struct UserAction
	{
		int id;
		std::vector<MappedInput> mappedButtons;
		std::vector<MappedAxis> mappedAxes;

		UserAction(int userActionId)
			: id(userActionId)
		{

		}
	};

	std::vector<UserAction> userActions;
};
