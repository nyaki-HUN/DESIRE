#pragma once

#include "Engine/UI/UI.h"

class DynamicMesh;
class Material;
class Texture;

struct nk_allocator;
struct nk_buffer;
struct nk_context;
struct nk_convert_config;
struct nk_font_atlas;

class NuklearUI : public UI
{
public:
	NuklearUI();
	~NuklearUI() override;

	void Init() override;
	void Kill() override;

	void NewFrame(OSWindow* pWindow) override;
	void Render() override;

	// Window
	void BeginWindow(const String& label, const Vector2& initialPos, const Vector2& initialSize) override;
	void EndWindow() override;

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

	// Layout
	void LayoutColumns(uint8_t numColumns, const float* pRatio = nullptr) override;

private:
	std::unique_ptr<nk_context> ctx;
	std::unique_ptr<nk_allocator> allocator;
	std::unique_ptr<nk_buffer> cmdBuffer;
	std::unique_ptr<nk_font_atlas> fontAtlas;
	std::unique_ptr<nk_convert_config> convertConfig;

	std::unique_ptr<DynamicMesh> mesh;
	std::unique_ptr<Material> material;
	std::shared_ptr<Texture> fontTexture;
};
