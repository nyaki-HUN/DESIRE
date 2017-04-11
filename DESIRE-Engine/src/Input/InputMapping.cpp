#include "stdafx.h"
#include "Input/InputMapping.h"
#include "Input/Input.h"

InputMapping::InputMapping()
{

}

void InputMapping::MapButton(int userActionId, const InputDevice& inputDevice, int buttonId)
{
	SUserAction& action = GetOrCreateUserActionById(userActionId);
	for(SMappedInput& mappedInput : action.mappedInputs)
	{
		if(mappedInput.buttonId == buttonId && mappedInput.inputDeviceHandle == inputDevice.handle)
		{
			// Button already mapped
			return;
		}
	}

	action.mappedInputs.push_back({ inputDevice.handle, buttonId });
}

bool InputMapping::IsDown(int userActionId) const
{
	const SUserAction *action = GetUserActionById(userActionId);
	for(const SMappedInput& mappedInput : action->mappedInputs)
	{
		const InputDevice *inputDevice = Input::Get()->GetInputDeviceByHandle(mappedInput.inputDeviceHandle);
		if(inputDevice != nullptr && inputDevice->IsDown(mappedInput.buttonId))
		{
			return true;
		}
	}

	return false;
}

bool InputMapping::WentDown(int userActionId) const
{
	const SUserAction *action = GetUserActionById(userActionId);
	for(const SMappedInput& mappedInput : action->mappedInputs)
	{
		const InputDevice *inputDevice = Input::Get()->GetInputDeviceByHandle(mappedInput.inputDeviceHandle);
		if(inputDevice != nullptr && inputDevice->WentDown(mappedInput.buttonId))
		{
			return true;
		}
	}

	return false;
}

uint8_t InputMapping::GetPressedCount(int userActionId) const
{
	uint8_t pressedCount = 0;

	const SUserAction *action = GetUserActionById(userActionId);
	for(const SMappedInput& mappedInput : action->mappedInputs)
	{
		const InputDevice *inputDevice = Input::Get()->GetInputDeviceByHandle(mappedInput.inputDeviceHandle);
		if(inputDevice != nullptr)
		{
			pressedCount += inputDevice->GetPressedCount(mappedInput.buttonId);
		}
	}

	return pressedCount;
}

const InputMapping::SUserAction* InputMapping::GetUserActionById(int userActionId) const
{
	for(const SUserAction& userAction : userActions)
	{
		if(userAction.id == userActionId)
		{
			return &userAction;
		}
	}

	return nullptr;
}

InputMapping::SUserAction& InputMapping::GetOrCreateUserActionById(int userActionId)
{
	for(SUserAction& userAction : userActions)
	{
		if(userAction.id == userActionId)
		{
			return userAction;
		}
	}

	userActions.emplace_back(userActionId);
	return userActions.back();
}
