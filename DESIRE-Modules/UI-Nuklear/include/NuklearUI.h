#pragma once

#include "Engine/UI/UI.h"

struct nk_allocator;
struct nk_context;

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

	nk_context* ctx = nullptr;
	std::unique_ptr<nk_allocator> allocator;
};
