#pragma once

#include "Engine/Core/Math/Vector2.h"

class OSWindow;
class String;
class Vector3;

class UI
{
public:
	virtual ~UI() {};

	virtual void Init() = 0;
	virtual void Kill() = 0;

	virtual void BeginFrame(OSWindow* window) = 0;
	virtual void EndFrame() = 0;
	virtual void Render() = 0;

	// Window
	virtual void BeginWindow(const String& label) = 0;
	virtual void EndWindow() = 0;

	// Widgets
	virtual void Text(const String& label) = 0;
	virtual bool Button(const String& label, const Vector2& size = Vector2::Zero()) = 0;
	virtual bool Checkbox(const String& label, bool& isChecked) = 0;

	template<typename T>
	bool RadioButton(const String& label, T& value, T buttonValue)
	{
		const bool pressed = RadioButton(label, value == buttonValue);
		if(pressed)
		{
			value = buttonValue;
		}
		return pressed;
	}

	virtual bool Slider(const String& label, int32_t& value, int32_t minValue, int32_t maxValue) = 0;
	virtual bool Slider(const String& label, float& value, float minValue, float maxValue) = 0;

	virtual bool InputField(const String& label, float& value) = 0;
	virtual bool InputField(const String& label, Vector3& value) = 0;

	// Layout
	virtual void SameLine() = 0;

private:
	virtual bool RadioButton(const String& label, bool isActive) = 0;
};