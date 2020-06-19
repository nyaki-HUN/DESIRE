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

	void BeginFrame(OSWindow* pWindow) override;
	void EndFrame() override;
	void Render() override;

	// Window
	void BeginWindow(const String& label) override;
	void EndWindow() override;

	// Widgets
	void Text(const String& label) override;
	bool Button(const String& label, const Vector2& size = Vector2::Zero()) override;
	bool Checkbox(const String& label, bool& isChecked) override;

	bool Slider(const String& label, int32_t& value, int32_t minValue, int32_t maxValue) override;
	bool Slider(const String& label, float& value, float minValue, float maxValue) override;

	bool InputField(const String& label, float& value) override;
	bool InputField(const String& label, Vector3& value) override;

	// Layout
	void SameLine() override;

private:
	bool RadioButton(const String& label, bool isActive) override;

	std::unique_ptr<DynamicMesh> mesh;
	std::unique_ptr<Material> material;
	std::shared_ptr<Texture> fontTexture;
};