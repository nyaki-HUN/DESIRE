#include "stdafx.h"
#include "Input/InputMapping.h"
#include "Input/Input.h"
#include "Core/STL_utils.h"

InputMapping::InputMapping()
{

}

void InputMapping::MapButton(int userActionId, const InputDevice& inputDevice, int buttonId)
{
	auto it = core::binary_find_or_insert(userActions, userActionId);
	for(SMappedInput& mappedInput : it->mappedButtons)
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
	it->mappedButtons.push_back(button);
}

void InputMapping::MapAxis(int userActionId, const InputDevice& inputDevice, int axisId)
{
	auto it = core::binary_find_or_insert(userActions, userActionId);
	for(SMappedInput& mappedInput : it->mappedAxes)
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
	it->mappedAxes.push_back(axis);
}

bool InputMapping::IsMapped(int userActionId) const
{
	auto it = core::binary_find(userActions, userActionId);
	return (it != userActions.end());
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
	auto it = core::binary_find(userActions, userActionId);
	if(it != userActions.end())
	{
		for(const SMappedInput& button : it->mappedButtons)
		{
			const InputDevice *inputDevice = Input::Get()->GetInputDeviceByHandle(button.inputDeviceHandle);
			if(inputDevice != nullptr && inputDevice->IsDown(button.id))
			{
				return true;
			}
		}
	}

	return false;
}

bool InputMapping::WentDown(int userActionId) const
{
	auto it = core::binary_find(userActions, userActionId);
	if(it != userActions.end())
	{
		for(const SMappedInput& button : it->mappedButtons)
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

	auto it = core::binary_find(userActions, userActionId);
	if(it != userActions.end())
	{
		for(const SMappedInput& button : it->mappedButtons)
		{
			const InputDevice *inputDevice = Input::Get()->GetInputDeviceByHandle(button.inputDeviceHandle);
			if(inputDevice != nullptr)
			{
				pressedCount += inputDevice->GetPressedCount(button.id);
			}
		}
	}

	return pressedCount;
}

/*
GetAxisState
{
	if(std::abs(value) <= axis->deadZone)
	{
		value = 0.0f;
	}
}
*/
