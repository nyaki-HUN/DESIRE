#include "Engine/stdafx.h"
#include "Engine/Input/InputMapping.h"
#include "Engine/Input/Input.h"

InputMapping::InputMapping()
{
}

void InputMapping::MapButton(int userActionId, const InputDevice& inputDevice, int buttonId)
{
	UserAction& userAction = GetOrAddUserActionById(userActionId);
	for(const MappedInput& mappedInput : userAction.mappedButtons)
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
	userAction.mappedButtons.Add(button);
}

void InputMapping::MapAxis(int userActionId, const InputDevice& inputDevice, int axisId)
{
	UserAction& userAction = GetOrAddUserActionById(userActionId);
	for(const MappedInput& mappedInput : userAction.mappedAxes)
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
	userAction.mappedAxes.Add(axis);
}

bool InputMapping::IsMapped(int userActionId) const
{
	const size_t idx = userActions.SpecializedFind([userActionId](const UserAction& userAction)
	{
		return (userAction.id == userActionId);
	});

	return (idx != SIZE_MAX);
}

void InputMapping::Unmap(int userActionId)
{
	const size_t idx = userActions.SpecializedFind([userActionId](const UserAction& userAction)
	{
		return (userAction.id == userActionId);
	});

	if(idx != SIZE_MAX)
	{
		userActions.RemoveFastAt(idx);
	}
}

bool InputMapping::IsDown(int userActionId) const
{
	return GetFloatState(userActionId) > 0.0f;
}

bool InputMapping::WentDown(int userActionId) const
{
	const size_t idx = userActions.SpecializedFind([userActionId](const UserAction& userAction)
	{
		return (userAction.id == userActionId);
	});

	if(idx != SIZE_MAX)
	{
		for(const MappedInput& button : userActions[idx].mappedButtons)
		{
			const InputDevice* inputDevice = Modules::Input->GetInputDeviceByHandle(button.inputDeviceHandle);
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

	const size_t idx = userActions.SpecializedFind([userActionId](const UserAction& userAction)
	{
		return (userAction.id == userActionId);
	});

	if(idx != SIZE_MAX)
	{
		for(const MappedInput& button : userActions[idx].mappedButtons)
		{
			const InputDevice* inputDevice = Modules::Input->GetInputDeviceByHandle(button.inputDeviceHandle);
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
	const size_t idx = userActions.SpecializedFind([userActionId](const UserAction& userAction)
	{
		return (userAction.id == userActionId);
	});

	if(idx != SIZE_MAX)
	{
		for(const MappedInput& button : userActions[idx].mappedButtons)
		{
			const InputDevice* inputDevice = Modules::Input->GetInputDeviceByHandle(button.inputDeviceHandle);
			if(inputDevice != nullptr && inputDevice->IsDown(button.id))
			{
				return 1.0f;
			}
		}

		for(const MappedAxis& axis : userActions[idx].mappedAxes)
		{
			const InputDevice* inputDevice = Modules::Input->GetInputDeviceByHandle(axis.inputDeviceHandle);
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

InputMapping::UserAction& InputMapping::GetOrAddUserActionById(int userActionId)
{
	const size_t idx = userActions.SpecializedFind([userActionId](const UserAction& userAction)
	{
		return (userAction.id == userActionId);
	});

	return (idx != SIZE_MAX) ? userActions[idx] : userActions.EmplaceAdd(userActionId);
}
