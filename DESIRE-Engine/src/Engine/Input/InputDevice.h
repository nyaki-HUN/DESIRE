#pragma once

class InputDevice
{
protected:
	InputDevice(void* pHandle, size_t offsetOfButtonStates, uint16_t numButtonStates, size_t offsetOfAxisStates = 0, uint16_t numAxisStates = 0);

public:
	bool IsDown(int32_t buttonId) const;
	bool WasPressed(int32_t buttonId) const;

	// Returns how many times the button went down since the last frame
	uint8_t GetPressedCount(int32_t buttonId) const;

	// Returns the current axis position, which is usually normalized into the [0.0, 1.0] range
	float GetAxisPos(int32_t axisId) const;

	// Returns the difference in axis position since the last frame
	float GetAxisDelta(int32_t axisId) const;

	void* GetHandle() const;

protected:
	struct AxisState
	{
		float delta = 0.0f;
		float pos = 0.0f;
	};

	void Update();
	void Reset();

	void HandleButton(int32_t buttonId, bool isDown);
	void HandleAxis(int32_t axisId, float delta);
	void HandleAxisAbsolute(int32_t axisId, float newPos);

	void* m_pHandle = nullptr;

	const uint16_t m_offsetOfButtonStatesInDerivedClass;
	const uint16_t m_offsetOfAxisStatesInDerivedClass;
	const uint16_t m_numButtonStates;
	const uint16_t m_numAxisStates;

private:
	static constexpr uint8_t kButtonStateDownFlag = 0x80;
};
