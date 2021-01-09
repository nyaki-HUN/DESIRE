#include "Engine/stdafx.h"
#include "Engine/Input/InputDevice.h"

InputDevice::InputDevice(void* pHandle, size_t offsetOfButtonStates, uint16_t numButtonStates, size_t offsetOfAxisStates, uint16_t numAxisStates)
	: m_pHandle(pHandle)
	, m_offsetOfButtonStatesInDerivedClass(static_cast<uint16_t>(offsetOfButtonStates))
	, m_numButtonStates(numButtonStates)
	, m_offsetOfAxisStatesInDerivedClass(static_cast<uint16_t>(offsetOfAxisStates))
	, m_numAxisStates(numAxisStates)
{
}

bool InputDevice::IsDown(int32_t buttonId) const
{
	if(static_cast<uint16_t>(buttonId) >= m_numButtonStates)
	{
		ASSERT(false && "Invalid button ID");
		return false;
	}

	const uint8_t buttonState = (reinterpret_cast<const uint8_t*>(this) + m_offsetOfButtonStatesInDerivedClass)[buttonId];
	return (buttonState & InputDevice::kButtonStateDownFlag) != 0;
}

bool InputDevice::WasPressed(int32_t buttonId) const
{
	return GetPressedCount(buttonId) != 0;
}

uint8_t InputDevice::GetPressedCount(int32_t buttonId) const
{
	if(static_cast<uint16_t>(buttonId) >= m_numButtonStates)
	{
		ASSERT(false && "Invalid button ID");
		return 0;
	}

	const uint8_t buttonState = (reinterpret_cast<const uint8_t*>(this) + m_offsetOfButtonStatesInDerivedClass)[buttonId];
	return static_cast<uint8_t>(buttonState & ~InputDevice::kButtonStateDownFlag);
}

float InputDevice::GetAxisPos(int32_t axisId) const
{
	if(static_cast<uint16_t>(axisId) >= m_numAxisStates)
	{
		ASSERT(false && "Invalid axis ID");
		return 0.0f;
	}

	const AxisState& axis = reinterpret_cast<const AxisState*>(reinterpret_cast<const uint8_t*>(this) + m_offsetOfAxisStatesInDerivedClass)[axisId];
	return axis.pos;
}

float InputDevice::GetAxisDelta(int32_t axisId) const
{
	if(static_cast<uint16_t>(axisId) >= m_numAxisStates)
	{
		ASSERT(false && "Invalid axis ID");
		return 0.0f;
	}

	const AxisState& axis = reinterpret_cast<const AxisState*>(reinterpret_cast<const uint8_t*>(this) + m_offsetOfAxisStatesInDerivedClass)[axisId];
	return axis.delta;
}

void* InputDevice::GetHandle() const
{
	return m_pHandle;
}

void InputDevice::Update()
{
	// Reset pressed count
	for(uint16_t i = 0; i < m_numButtonStates; ++i)
	{
		uint8_t& buttonState = (reinterpret_cast<uint8_t*>(this) + m_offsetOfButtonStatesInDerivedClass)[i];
		buttonState &= InputDevice::kButtonStateDownFlag;
	}

	// Reset axis deltas
	for(uint16_t i = 0; i < m_numAxisStates; ++i)
	{
		AxisState& axis = reinterpret_cast<AxisState*>(reinterpret_cast<uint8_t*>(this) + m_offsetOfAxisStatesInDerivedClass)[i];
		axis.delta = 0.0f;
	}
}

void InputDevice::Reset()
{
	if(m_numButtonStates != 0)
	{
		uint8_t* buttonStates = reinterpret_cast<uint8_t*>(this) + m_offsetOfButtonStatesInDerivedClass;
		memset(buttonStates, 0, m_numButtonStates * sizeof(uint8_t));
	}

	for(uint16_t i = 0; i < m_numAxisStates; ++i)
	{
		AxisState& axis = reinterpret_cast<AxisState*>(reinterpret_cast<uint8_t*>(this) + m_offsetOfAxisStatesInDerivedClass)[i];
		axis.delta = 0.0f;
	}
}

void InputDevice::HandleButton(int32_t buttonId, bool isDown)
{
	ASSERT(buttonId >= 0 && buttonId < m_numButtonStates);

	uint8_t& buttonState = (reinterpret_cast<uint8_t*>(this) + m_offsetOfButtonStatesInDerivedClass)[buttonId];
	if(isDown)
	{
		if((buttonState & InputDevice::kButtonStateDownFlag) == 0)
		{
			buttonState++;
		}

		buttonState |= InputDevice::kButtonStateDownFlag;
	}
	else
	{
		buttonState &= ~InputDevice::kButtonStateDownFlag;
	}
}

void InputDevice::HandleAxis(int32_t axisId, float delta)
{
	ASSERT(axisId >= 0 && axisId < m_numAxisStates);

	AxisState& axis = reinterpret_cast<AxisState*>(reinterpret_cast<uint8_t*>(this) + m_offsetOfAxisStatesInDerivedClass)[axisId];
	axis.delta += delta;
	axis.pos += delta;
}

void InputDevice::HandleAxisAbsolute(int32_t axisId, float newPos)
{
	ASSERT(axisId >= 0 && axisId < m_numAxisStates);

	AxisState& axis = reinterpret_cast<AxisState*>(reinterpret_cast<uint8_t*>(this) + m_offsetOfAxisStatesInDerivedClass)[axisId];
	axis.delta += newPos - axis.pos;
	axis.pos = newPos;
}
