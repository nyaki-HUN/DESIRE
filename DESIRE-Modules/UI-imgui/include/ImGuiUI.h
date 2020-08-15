#pragma once

#include "Engine/UI/UI.h"

class DynamicMesh;
class Material;
class Texture;

class ImGuiUI : public UI
{
public:
	ImGuiUI();
	~ImGuiUI() override;

	void Init() override;
	void Kill() override;

	void NewFrame(OSWindow* pWindow) override;
	void Render() override;

	// Window
	void BeginWindow(const String& label, const Vector2& initialPos, const Vector2& initialSize) override;
	void EndWindow() override;

	// Widgets
	void Text(const String& label) override;
	bool TextInput(const String& label, WritableString& value) override;
	bool Button(const String& label, const Vector2& size = Vector2::Zero()) override;
	bool ArrowButton(const String& label, EArrowDir dir) override;
	bool Checkbox(const String& label, bool& isChecked) override;
	bool RadioButtonOption(const String& label, bool isActive) override;

	bool ValueSpinner(const String& label, int32_t& value, int32_t minValue = INT32_MIN, int32_t maxValue = INT32_MAX, float speed = 0.1f) override;
	bool ValueSpinner(const String& label, float& value, float minValue = -FLT_MAX, float maxValue = FLT_MAX, float step = 0.01f) override;

	bool ValueEdit(const String& label, Vector3& value) override;

	bool Slider(const String& label, int32_t& value, int32_t minValue, int32_t maxValue) override;
	bool Slider(const String& label, float& value, float minValue, float maxValue) override;

	void ProgressBar(float progress) override;

	bool ColorPicker(const String& label, float(&colorRGB)[3]) override;
	bool ColorPicker(const String& label, float(&colorRGBA)[4]) override;

	// Layout
	void SameLine() override;

private:
	std::unique_ptr<DynamicMesh> mesh;
	std::unique_ptr<Material> material;
	std::shared_ptr<Texture> fontTexture;
};
