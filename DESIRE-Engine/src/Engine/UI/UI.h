#pragma once

#include "Engine/Core/Math/Vector2.h"

class OSWindow;
class String;
class Vector3;
class WritableString;

class UI
{
public:
	enum class EArrowDir
	{
		Left,
		Right,
		Up,
		Down
	};

	virtual ~UI() {};

	virtual void Init() = 0;
	virtual void Kill() = 0;

	virtual void NewFrame(OSWindow* pWindow) = 0;
	virtual void Render() = 0;

	// Window
	virtual void BeginWindow(const String& label, const Vector2& initialPos, const Vector2& initialSize) = 0;
	virtual void EndWindow() = 0;

	// Widgets
	virtual void Text(const String& label) = 0;
	virtual bool TextInput(const String& label, WritableString& value) = 0;
	virtual bool Button(const String& label, const Vector2& size = Vector2::Zero()) = 0;
	virtual bool ArrowButton(const String& label, EArrowDir dir) = 0;
	virtual bool Checkbox(const String& label, bool& isChecked) = 0;
	virtual bool RadioButtonOption(const String& label, bool isActive) = 0;

	template<typename T>
	bool RadioButton(const String& label, T& value, T buttonValue)
	{
		if(RadioButtonOption(label, value == buttonValue))
		{
			value = buttonValue;
			return true;
		}
		return false;
	}

	virtual bool ValueSpinner(const String& label, int32_t& value, int32_t minValue = INT32_MIN, int32_t maxValue = INT32_MAX, int32_t step = 1) = 0;
	virtual bool ValueSpinner(const String& label, float& value, float minValue = -FLT_MAX, float maxValue = FLT_MAX, float step = 0.01f) = 0;

	virtual bool ValueEdit(const String& label, Vector3& value) = 0;

	virtual bool Slider(const String& label, int32_t& value, int32_t minValue, int32_t maxValue) = 0;
	virtual bool Slider(const String& label, float& value, float minValue, float maxValue) = 0;

	virtual void ProgressBar(float progress) = 0;

	virtual bool ColorPicker(const String& label, float(&colorRGB)[3]) = 0;
	virtual bool ColorPicker(const String& label, float(&colorRGBA)[4]) = 0;

	// Layout
	virtual void SameLine() = 0;
};
