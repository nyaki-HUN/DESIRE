#pragma once

class OSWindow;
class String;
class Vector2;
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

	enum EWindowFlags
	{
		WindowFlags_None			= 0,
		WindowFlags_NoTitleBar		= 1 << 0,
		WindowFlags_NoResize		= 1 << 1,
		WindowFlags_NoMove			= 1 << 2,
		WindowFlags_NoScrollbar		= 1 << 3,
	};

	virtual ~UI() {}

	virtual void Init() = 0;
	virtual void Kill() = 0;

	virtual void NewFrame(OSWindow& window) = 0;
	virtual void Render() = 0;

	// Window
	virtual bool BeginWindow(const String& label, const Vector2& initialPos, const Vector2& initialSize, bool* pOpen = nullptr, EWindowFlags flags = WindowFlags_None) = 0;
	virtual void EndWindow() = 0;

	// Table
	virtual bool BeginTable(const String& id, uint8_t numColumns, const float* pInitialColumnsRatio = nullptr) = 0;
	virtual void EndTable() = 0;

	// Widgets
	virtual void Text(const String& label) = 0;
	virtual bool TextInput(WritableString& value) = 0;

	bool Button(const String& label);
	virtual bool Button(const String& label, const Vector2& size) = 0;
	virtual bool ArrowButton(EArrowDir dir) = 0;

	bool Checkbox(bool& isChecked);
	virtual bool Checkbox(bool& isChecked, const String& label) = 0;

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

	virtual bool RadioButtonOption(const String& label, bool isActive) = 0;

	virtual bool ValueSpinner(int32_t& value, int32_t minValue = INT32_MIN, int32_t maxValue = INT32_MAX, float speed = 0.1f) = 0;
	virtual bool ValueSpinner(float& value, float minValue = -FLT_MAX, float maxValue = FLT_MAX, float speed = 0.01f) = 0;

	virtual bool ValueEdit(float& value) = 0;
	virtual bool ValueEdit(Vector3& value) = 0;

	virtual bool Slider(int32_t& value, int32_t minValue, int32_t maxValue) = 0;
	virtual bool Slider(float& value, float minValue, float maxValue) = 0;

	virtual void ProgressBar(float progressPercent) = 0;

	virtual bool ColorPicker(float(&colorRGB)[3]) = 0;
	virtual bool ColorPicker(float(&colorRGBA)[4]) = 0;
};
