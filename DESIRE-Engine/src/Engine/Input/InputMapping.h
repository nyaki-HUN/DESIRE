#pragma once

#include "Engine/Core/Container/Array.h"

class InputDevice;

class InputMapping
{
public:
	InputMapping();

	void MapButton(int32_t userActionId, const InputDevice& inputDevice, int32_t buttonId);
	void MapAxis(int32_t userActionId, const InputDevice& inputDevice, int32_t axisId);
	bool IsMapped(int32_t userActionId) const;

	// Removes all mappings for the given user action
	void Unmap(int32_t userActionId);

	bool IsDown(int32_t userActionId) const;
	bool WasPressed(int32_t userActionId) const;

	// Returns how many times the action was triggered since the last frame
	uint8_t GetPressedCount(int32_t userActionId) const;

	float GetFloatState(int32_t userActionId) const;

private:
	struct MappedInput
	{
		int32_t id = 0;
		const void* pInputDeviceHandle = nullptr;
	};

	struct MappedAxis : public MappedInput
	{
		float deadZone = 0.0f;
	};

	struct UserAction
	{
		int32_t id;
		Array<MappedInput> mappedButtons;
		Array<MappedAxis> mappedAxes;

		UserAction(int32_t userActionId)
			: id(userActionId)
		{

		}
	};

	UserAction& GetOrAddUserActionById(int32_t userActionId);

	Array<UserAction> m_userActions;
};
