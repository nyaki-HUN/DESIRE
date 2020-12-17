#pragma once

#include "Engine/UI/UI.h"

class Material;
class Renderable;
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

	void NewFrame(OSWindow& window) override;
	void Render() override;

	// Window
	bool BeginWindow(const String& label, const Vector2& initialPos, const Vector2& initialSize, bool* pOpen = nullptr, EWindowFlags flags = WindowFlags_None) override;
	void EndWindow() override;

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
	std::unique_ptr<nk_context> m_spContext;
	std::unique_ptr<nk_allocator> m_spAllocator;
	std::unique_ptr<nk_buffer> m_spCmdBuffer;
	std::unique_ptr<nk_font_atlas> m_spFontAtlas;
	std::unique_ptr<nk_convert_config> m_spConvertConfig;

	std::unique_ptr<Renderable> m_spRenderable;
	std::shared_ptr<Texture> m_spFontTexture;
};
