#include "stdafx.h"
#include "Input/InputMap.h"
#include "Input/Input.h"

InputMap::InputMap()
{

}

void InputMap::MapButton(int userActionId, const InputDevice& inputDevice, int buttonId)
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

bool InputMap::IsDown(int userActionId) const
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

bool InputMap::WentDown(int userActionId) const
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

// Returns how many times the action was triggered since the last frame
uint8_t InputMap::GetPressedCount(int userActionId) const
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

const InputMap::SUserAction* InputMap::GetUserActionById(int userActionId) const
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

InputMap::SUserAction& InputMap::GetOrCreateUserActionById(int userActionId)
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
