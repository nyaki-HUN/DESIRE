#include "Engine/stdafx.h"
#include "Engine/Input/InputMapping.h"

#include "Engine/Input/Input.h"

InputMapping::InputMapping()
{
}

void InputMapping::MapButton(int32_t userActionId, const InputDevice& inputDevice, int32_t buttonId)
{
	UserAction& userAction = GetOrAddUserActionById(userActionId);
	for(const MappedInput& mappedInput : userAction.mappedButtons)
	{
		if(mappedInput.id == buttonId && mappedInput.pInputDeviceHandle == inputDevice.GetHandle())
		{
			// Already mapped
			return;
		}
	}

	MappedInput button;
	button.id = buttonId;
	button.pInputDeviceHandle = inputDevice.GetHandle();
	userAction.mappedButtons.Add(button);
}

void InputMapping::MapAxis(int32_t userActionId, const InputDevice& inputDevice, int32_t axisId)
{
	UserAction& userAction = GetOrAddUserActionById(userActionId);
	for(const MappedInput& mappedInput : userAction.mappedAxes)
	{
		if(mappedInput.id == axisId && mappedInput.pInputDeviceHandle == inputDevice.GetHandle())
		{
			// Already mapped
			return;
		}
	}

	MappedAxis axis;
	axis.id = axisId;
	axis.pInputDeviceHandle = inputDevice.GetHandle();
	axis.deadZone = 0;
	userAction.mappedAxes.Add(axis);
}

bool InputMapping::IsMapped(int32_t userActionId) const
{
	const size_t idx = m_userActions.SpecializedFind([userActionId](const UserAction& userAction)
	{
		return (userAction.id == userActionId);
	});

	return (idx != SIZE_MAX);
}

void InputMapping::Unmap(int32_t userActionId)
{
	const size_t idx = m_userActions.SpecializedFind([userActionId](const UserAction& userAction)
	{
		return (userAction.id == userActionId);
	});

	if(idx != SIZE_MAX)
	{
		m_userActions.RemoveFastAt(idx);
	}
}

bool InputMapping::IsDown(int32_t userActionId) const
{
	return GetFloatState(userActionId) > 0.0f;
}

bool InputMapping::WasPressed(int32_t userActionId) const
{
	const size_t idx = m_userActions.SpecializedFind([userActionId](const UserAction& userAction)
	{
		return (userAction.id == userActionId);
	});

	if(idx != SIZE_MAX)
	{
		for(const MappedInput& button : m_userActions[idx].mappedButtons)
		{
			const InputDevice* pInputDevice = Modules::Input->GetInputDeviceByHandle(button.pInputDeviceHandle);
			if(pInputDevice && pInputDevice->WasPressed(button.id))
			{
				return true;
			}
		}
	}

	return false;
}

uint8_t InputMapping::GetPressedCount(int32_t userActionId) const
{
	uint8_t pressedCount = 0;

	const size_t idx = m_userActions.SpecializedFind([userActionId](const UserAction& userAction)
	{
		return (userAction.id == userActionId);
	});

	if(idx != SIZE_MAX)
	{
		for(const MappedInput& button : m_userActions[idx].mappedButtons)
		{
			const InputDevice* pInputDevice = Modules::Input->GetInputDeviceByHandle(button.pInputDeviceHandle);
			if(pInputDevice)
			{
				pressedCount += pInputDevice->GetPressedCount(button.id);
			}
		}
	}

	return pressedCount;
}

float InputMapping::GetFloatState(int32_t userActionId) const
{
	const size_t idx = m_userActions.SpecializedFind([userActionId](const UserAction& userAction)
	{
		return (userAction.id == userActionId);
	});

	if(idx != SIZE_MAX)
	{
		for(const MappedInput& button : m_userActions[idx].mappedButtons)
		{
			const InputDevice* pInputDevice = Modules::Input->GetInputDeviceByHandle(button.pInputDeviceHandle);
			if(pInputDevice && pInputDevice->IsDown(button.id))
			{
				return 1.0f;
			}
		}

		for(const MappedAxis& axis : m_userActions[idx].mappedAxes)
		{
			const InputDevice* pInputDevice = Modules::Input->GetInputDeviceByHandle(axis.pInputDeviceHandle);
			if(pInputDevice)
			{
				float pos = pInputDevice->GetAxisPos(axis.id);
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

InputMapping::UserAction& InputMapping::GetOrAddUserActionById(int32_t userActionId)
{
	const size_t idx = m_userActions.SpecializedFind([userActionId](const UserAction& userAction)
	{
		return (userAction.id == userActionId);
	});

	return (idx != SIZE_MAX) ? m_userActions[idx] : m_userActions.EmplaceAdd(userActionId);
}
