#include "stdafx.h"
#include "Input/InputMapping.h"
#include "Input/Input.h"
#include "Core/STL_utils.h"

InputMapping::InputMapping()
{

}

void InputMapping::MapButton(int userActionId, const InputDevice& inputDevice, int buttonId)
{
	SUserAction& action = core::binary_search_or_insert(userActions, SUserAction(userActionId));
	for(SMappedInput& mappedInput : action.mappedButtons)
	{
		if(mappedInput.id == buttonId && mappedInput.inputDeviceHandle == inputDevice.handle)
		{
			// Already mapped
			return;
		}
	}

	SMappedInput button;
	button.id = buttonId;
	button.inputDeviceHandle = inputDevice.handle;
	action.mappedButtons.push_back(button);
}

void InputMapping::MapAxis(int userActionId, const InputDevice& inputDevice, int axisId)
{
	SUserAction& action = core::binary_search_or_insert(userActions, SUserAction(userActionId));
	for(SMappedInput& mappedInput : action.mappedAxes)
	{
		if(mappedInput.id == axisId && mappedInput.inputDeviceHandle == inputDevice.handle)
		{
			// Already mapped
			return;
		}
	}

	SMappedAxis axis;
	axis.id = axisId;
	axis.inputDeviceHandle = inputDevice.handle;
	axis.deadZone = 0.0f;
	action.mappedAxes.push_back(axis);
}

bool InputMapping::IsMapped(int userActionId) const
{
	const SUserAction *action = core::binary_search(userActions, SUserAction(userActionId));
	return (action != nullptr);
}

void InputMapping::Unmap(int userActionId)
{
	userActions.erase(std::remove_if(userActions.begin(), userActions.end(), [userActionId](const SUserAction& action)
	{
		return (action.id == userActionId);
	}), userActions.end());
}

bool InputMapping::IsDown(int userActionId) const
{
	const SUserAction *action = core::binary_search(userActions, SUserAction(userActionId));
	if(action != nullptr)
	{
		for(const SMappedInput& mappedInput : action->mappedButtons)
		{
			const InputDevice *inputDevice = Input::Get()->GetInputDeviceByHandle(mappedInput.inputDeviceHandle);
			if(inputDevice != nullptr && inputDevice->IsDown(mappedInput.id))
			{
				return true;
			}
		}
	}

	return false;
}

bool InputMapping::WentDown(int userActionId) const
{
	const SUserAction *action = core::binary_search(userActions, SUserAction(userActionId));
	if(action != nullptr)
	{
		for(const SMappedInput& button : action->mappedButtons)
		{
			const InputDevice *inputDevice = Input::Get()->GetInputDeviceByHandle(button.inputDeviceHandle);
			if(inputDevice != nullptr && inputDevice->WentDown(button.id))
			{
				return true;
			}
		}
	}

	return false;
}

uint8_t InputMapping::GetPressedCount(int userActionId) const
{
	uint8_t pressedCount = 0;

	const SUserAction *action = core::binary_search(userActions, SUserAction(userActionId));
	if(action != nullptr)
	{
		for(const SMappedInput& mappedInput : action->mappedButtons)
		{
			const InputDevice *inputDevice = Input::Get()->GetInputDeviceByHandle(mappedInput.inputDeviceHandle);
			if(inputDevice != nullptr)
			{
				pressedCount += inputDevice->GetPressedCount(mappedInput.id);
			}
		}
	}

	return pressedCount;
}
