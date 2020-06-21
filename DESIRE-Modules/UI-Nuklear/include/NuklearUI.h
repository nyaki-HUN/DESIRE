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

	std::unique_ptr<nk_context> ctx;
	std::unique_ptr<nk_allocator> allocator;
	std::unique_ptr<nk_buffer> cmdBuffer;
	std::unique_ptr<nk_font_atlas> fontAtlas;
	std::unique_ptr<nk_convert_config> convertConfig;

	std::unique_ptr<DynamicMesh> mesh;
	std::unique_ptr<Material> material;
	std::shared_ptr<Texture> fontTexture;
};
