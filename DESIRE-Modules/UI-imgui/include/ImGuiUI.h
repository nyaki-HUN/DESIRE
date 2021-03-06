#pragma once

#include "Engine/UI/UI.h"

class Renderable;
class Texture;

class ImGuiUI : public UI
{
public:
	ImGuiUI();
	~ImGuiUI() override;

	void Init() override;
	void Kill() override;

	void NewFrame(OSWindow& window) override;
	void Render() override;

	// Window
	bool BeginWindow(const String& label, const Vector2& initialPos, const Vector2& initialSize, bool* pOpen = nullptr, EWindowFlags flags = WindowFlags_None) override;
	void EndWindow() override;

	// Menu
	bool BeginMenuBar() override;
	void EndMenuBar() override;

	bool BeginMenu(const String& label) override;
	void EndMenu() override;

	bool MenuItem(const String& label) override;

	// Table
	bool BeginTable(const String& id, uint8_t numColumns, const float* pInitialColumnsRatio = nullptr) override;
	void EndTable() override;

	// Widgets
	void Text(const String& label) override;
	bool TextInput(WritableString& value) override;

	bool Button(const String& label, const Vector2& size) override;
	bool ArrowButton(EArrowDir dir) override;

	bool Checkbox(bool& isChecked, const String& label) override;

	bool RadioButtonOption(const String& label, bool isActive) override;

	bool ValueSpinner(int32_t& value, int32_t minValue = INT32_MIN, int32_t maxValue = INT32_MAX, float speed = 0.1f) override;
	bool ValueSpinner(float& value, float minValue = -FLT_MAX, float maxValue = FLT_MAX, float speed = 0.01f) override;

	bool ValueEdit(float& value) override;
	bool ValueEdit(Vector3& value) override;

	bool Slider(int32_t& value, int32_t minValue, int32_t maxValue) override;
	bool Slider(float& value, float minValue, float maxValue) override;

	void ProgressBar(float progressPercent) override;

	bool ColorPicker(float(&colorRGB)[3]) override;
	bool ColorPicker(float(&colorRGBA)[4]) override;

private:
	std::unique_ptr<Renderable> m_spRenderable;
	std::shared_ptr<Texture> m_spFontTexture;
};
