#pragma once

#include "Engine/Core/Math/Vector2.h"

class String;
class Vector3;

class UI
{
public:
	// Widgets: Label
	static void Label(const String& label);

	// Widgets: Main
	static bool Button(const String& label, const Vector2& size = Vector2::Zero());
	static bool Checkbox(const String& label, bool& isChecked);

	template<typename T>
	static bool RadioButton(const String& label, T& value, T buttonValue)
	{
		const bool pressed = UI::RadioButton(label, value == buttonValue);
		if(pressed)
		{
			value = buttonValue;
		}
		return pressed;
	}

	// Widgets: Sliders
	static bool Slider(const String& label, int32_t& value, int32_t minValue, int32_t maxValue);
	static bool Slider(const String& label, float& value, float minValue, float maxValue);

	// Widgets: Input with Keyboard
	static bool InputField(const String& label, float& value);
	static bool InputField(const String& label, Vector3& value);

	// Layout
	static void SameLine();

private:
	static bool RadioButton(const String& label, bool isActive);
};
