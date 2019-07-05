#include "Engine/stdafx.h"
#include "Engine/Input/InputDevice.h"

InputDevice::InputDevice(void* handle, size_t offsetOfButtonStates, uint16_t numButtonStates, size_t offsetOfAxisStates, uint16_t numAxisStates)
	: handle(handle)
	, offsetOfButtonStatesInDerivedClass((uint16_t)offsetOfButtonStates)
	, numButtonStates(numButtonStates)
	, offsetOfAxisStatesInDerivedClass((uint16_t)offsetOfAxisStates)
	, numAxisStates(numAxisStates)
{
	Reset();
}

bool InputDevice::IsDown(int buttonId) const
{
	if((uint16_t)buttonId >= numButtonStates)
	{
		ASSERT(false && "Invalid button ID");
		return false;
	}

	const uint8_t buttonState = ((uint8_t*)this + offsetOfButtonStatesInDerivedClass)[buttonId];
	return (buttonState & InputDevice::BUTTON_STATE_DOWN_FLAG) != 0;
}

bool InputDevice::WentDown(int buttonId) const
{
	return GetPressedCount(buttonId) != 0;
}

uint8_t InputDevice::GetPressedCount(int buttonId) const
{
	if((uint16_t)buttonId >= numButtonStates)
	{
		ASSERT(false && "Invalid button ID");
		return 0;
	}

	const uint8_t buttonState = (reinterpret_cast<const uint8_t*>(this) + offsetOfButtonStatesInDerivedClass)[buttonId];
	return (uint8_t)(buttonState & ~InputDevice::BUTTON_STATE_DOWN_FLAG);
}

float InputDevice::GetAxisPos(int axisId) const
{
	if((uint16_t)axisId >= numAxisStates)
	{
		ASSERT(false && "Invalid axis ID");
		return 0.0f;
	}

	const AxisState& axis = reinterpret_cast<const AxisState*>(reinterpret_cast<const uint8_t*>(this) + offsetOfAxisStatesInDerivedClass)[axisId];
	return axis.pos;
}

float InputDevice::GetAxisDelta(int axisId) const
{
	if((uint16_t)axisId >= numAxisStates)
	{
		ASSERT(false && "Invalid axis ID");
		return 0.0f;
	}

	const AxisState& axis = reinterpret_cast<const AxisState*>(reinterpret_cast<const uint8_t*>(this) + offsetOfAxisStatesInDerivedClass)[axisId];
	return axis.delta;
}

void InputDevice::Update()
{
	// Reset pressed count
	for(uint16_t i = 0; i < numButtonStates; ++i)
	{
		uint8_t& buttonState = (reinterpret_cast<uint8_t*>(this) + offsetOfButtonStatesInDerivedClass)[i];
		buttonState &= InputDevice::BUTTON_STATE_DOWN_FLAG;
	}

	// Reset axis deltas
	for(uint16_t i = 0; i < numAxisStates; ++i)
	{
		AxisState& axis = reinterpret_cast<AxisState*>(reinterpret_cast<uint8_t*>(this) + offsetOfAxisStatesInDerivedClass)[i];
		axis.delta = 0.0f;
	}
}

void InputDevice::Reset()
{
	uint8_t* buttonStates = (uint8_t*)this + offsetOfButtonStatesInDerivedClass;
	memset(buttonStates, 0, numButtonStates * sizeof(uint8_t));
}

void InputDevice::HandleButton(int buttonId, bool isDown)
{
	ASSERT(buttonId >= 0 && buttonId < numButtonStates);

	uint8_t& buttonState = (reinterpret_cast<uint8_t*>(this) + offsetOfButtonStatesInDerivedClass)[buttonId];
	if(isDown)
	{
		if((buttonState & InputDevice::BUTTON_STATE_DOWN_FLAG) == 0)
		{
			buttonState++;
		}

		buttonState |= InputDevice::BUTTON_STATE_DOWN_FLAG;
	}
	else
	{
		buttonState &= ~InputDevice::BUTTON_STATE_DOWN_FLAG;
	}
}

void InputDevice::HandleAxis(int axisId, float delta)
{
	ASSERT(axisId >= 0 && axisId < numAxisStates);

	AxisState& axis = reinterpret_cast<AxisState*>(reinterpret_cast<uint8_t*>(this) + offsetOfAxisStatesInDerivedClass)[axisId];
	axis.delta += delta;
	axis.pos += delta;
}

void InputDevice::HandleAxisAbsolute(int axisId, float newPos)
{
	ASSERT(axisId >= 0 && axisId < numAxisStates);

	AxisState& axis = reinterpret_cast<AxisState*>(reinterpret_cast<uint8_t*>(this) + offsetOfAxisStatesInDerivedClass)[axisId];
	axis.delta += newPos - axis.pos;
	axis.pos = newPos;
}
