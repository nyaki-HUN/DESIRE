#include "Engine/stdafx.h"
#include "Engine/Input/InputMapping.h"
#include "Engine/Input/Input.h"
#include "Engine/Core/STL_utils.h"

InputMapping::InputMapping()
{

}

void InputMapping::MapButton(int userActionId, const InputDevice& inputDevice, int buttonId)
{
	auto it = stl_utils::binary_find_or_insert(userActions, userActionId);
	for(MappedInput& mappedInput : it->mappedButtons)
	{
		if(mappedInput.id == buttonId && mappedInput.inputDeviceHandle == inputDevice.handle)
		{
			// Already mapped
			return;
		}
	}

	MappedInput button;
	button.id = buttonId;
	button.inputDeviceHandle = inputDevice.handle;
	it->mappedButtons.push_back(button);
}

void InputMapping::MapAxis(int userActionId, const InputDevice& inputDevice, int axisId)
{
	auto it = stl_utils::binary_find_or_insert(userActions, userActionId);
	for(MappedInput& mappedInput : it->mappedAxes)
	{
		if(mappedInput.id == axisId && mappedInput.inputDeviceHandle == inputDevice.handle)
		{
			// Already mapped
			return;
		}
	}

	MappedAxis axis;
	axis.id = axisId;
	axis.inputDeviceHandle = inputDevice.handle;
	axis.deadZone = 0;
	it->mappedAxes.push_back(axis);
}

bool InputMapping::IsMapped(int userActionId) const
{
	auto it = stl_utils::binary_find(userActions, userActionId);
	return (it != userActions.end());
}

void InputMapping::Unmap(int userActionId)
{
	userActions.erase(std::remove_if(userActions.begin(), userActions.end(), [userActionId](const UserAction& action)
	{
		return (action.id == userActionId);
	}), userActions.end());
}

bool InputMapping::IsDown(int userActionId) const
{
	return GetFloatState(userActionId) > 0.0f;
}

bool InputMapping::WentDown(int userActionId) const
{
	auto it = stl_utils::binary_find(userActions, userActionId);
	if(it != userActions.end())
	{
		for(const MappedInput& button : it->mappedButtons)
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

	auto it = stl_utils::binary_find(userActions, userActionId);
	if(it != userActions.end())
	{
		for(const MappedInput& button : it->mappedButtons)
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

float InputMapping::GetFloatState(int userActionId) const
{
	auto it = stl_utils::binary_find(userActions, userActionId);
	if(it != userActions.end())
	{
		for(const MappedInput& button : it->mappedButtons)
		{
			const InputDevice *inputDevice = Input::Get()->GetInputDeviceByHandle(button.inputDeviceHandle);
			if(inputDevice != nullptr && inputDevice->IsDown(button.id))
			{
				return 1.0f;
			}
		}

		for(const MappedAxis& axis : it->mappedAxes)
		{
			const InputDevice *inputDevice = Input::Get()->GetInputDeviceByHandle(axis.inputDeviceHandle);
			if(inputDevice != nullptr)
			{
				float pos = inputDevice->GetAxisPos(axis.id);
				if(pos > 0.0f)
				{
					if(axis.deadZone != 0.0f)
					{
						if(std::abs(pos) <= axis.deadZone)
						{
							continue;
						}

						pos -= std::signbit(pos) ? -axis.deadZone : axis.deadZone;
						pos /= 1.0f - axis.deadZone;
					}

					return pos;
				}
			}
		}
	}

	return 0.0f;
}
