#include "stdafx.h"
#include "Input/InputDevice.h"

InputDevice::InputDevice(void *handle, size_t offsetOfButtonStates, size_t numButtonStates, int firstButtonId)
	: handle(handle)
	, offsetOfButtonStatesInDerivedClass(offsetOfButtonStates)
	, numButtonStates(numButtonStates)
	, firstButtonId(firstButtonId)
{

}

bool InputDevice::IsDown(int buttonId) const
{
	const size_t idx = buttonId - firstButtonId;
	if(idx >= numButtonStates)
	{
		ASSERT(false && "Invalid button ID");
		return false;
	}

	const uint8_t buttonState = ((uint8_t*)this + offsetOfButtonStatesInDerivedClass)[idx];
	return (buttonState & InputDevice::BUTTON_STATE_DOWN_FLAG) != 0;
}

bool InputDevice::WentDown(int buttonId) const
{
	return GetPressedCount(buttonId) != 0;
}

uint8_t InputDevice::GetPressedCount(int buttonId) const
{
	const size_t idx = buttonId - firstButtonId;
	if(idx >= numButtonStates)
	{
		ASSERT(false && "Invalid button ID");
		return false;
	}

	const uint8_t buttonState = ((uint8_t*)this + offsetOfButtonStatesInDerivedClass)[idx];
	return (uint8_t)(buttonState & ~InputDevice::BUTTON_STATE_DOWN_FLAG);
}

void InputDevice::SetButtonStateDown(int buttonId, bool isDown)
{
	const size_t idx = buttonId - firstButtonId;
	ASSERT(idx < numButtonStates);

	uint8_t& buttonState = ((uint8_t*)this + offsetOfButtonStatesInDerivedClass)[idx];
	if(isDown)
	{
		buttonState++;
		buttonState |= InputDevice::BUTTON_STATE_DOWN_FLAG;
	}
	else
	{
		buttonState &= ~InputDevice::BUTTON_STATE_DOWN_FLAG;
	}
}
