#include "stdafx_Nuklear.h"
#include "NuklearUI.h"

#include "NuklearCallbacks.h"

#include "Engine/Application/Application.h"
#include "Engine/Application/OSWindow.h"

#include "Engine/Core/Math/Vector3.h"
#include "Engine/Core/Memory/MemorySystem.h"
#include "Engine/Core/String/String.h"
#include "Engine/Core/Timer.h"

#include "Engine/Input/Input.h"

#include "Engine/Render/Material.h"
#include "Engine/Render/Render.h"

#include "Engine/Resource/ResourceManager.h"
#include "Engine/Resource/Mesh.h"
#include "Engine/Resource/Shader.h"
#include "Engine/Resource/Texture.h"

NuklearUI::NuklearUI()
	: allocator(std::make_unique<nk_allocator>())
{
	allocator->alloc = &NuklearCallbacks::MallocWrapper;
	allocator->free = &NuklearCallbacks::FreeWrapper;
	allocator->userdata = nk_handle_ptr(this);
}

NuklearUI::~NuklearUI()
{
}

void NuklearUI::Init()
{
	ctx = std::make_unique<nk_context>();
	nk_init(ctx.get(), allocator.get(), nullptr);

	// Dynamic mesh for the draw list
	const std::initializer_list<Mesh::VertexLayout> vertexLayout =
	{
		{ Mesh::EAttrib::Position,	2, Mesh::EAttribType::Float },
		{ Mesh::EAttrib::Texcoord0,	2, Mesh::EAttribType::Float },
		{ Mesh::EAttrib::Color,		4, Mesh::EAttribType::Uint8 }
	};
	mesh = std::make_unique<DynamicMesh>(vertexLayout, 128 * 1024, 256 * 1024);
	static_assert(sizeof(nk_draw_index) == sizeof(uint16_t), "Conversion is required for index buffer");

	static const nk_draw_vertex_layout_element s_nkVertexLayout[] =
	{
		{ NK_VERTEX_POSITION, NK_FORMAT_FLOAT, 0 },
		{ NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, 2 * sizeof(float) },
		{ NK_VERTEX_COLOR, NK_FORMAT_R8G8B8A8, 4 * sizeof(float) },
		{ NK_VERTEX_LAYOUT_END }
	};

	convertConfig = std::make_unique<nk_convert_config>();
	convertConfig->vertex_layout = s_nkVertexLayout;
	convertConfig->vertex_size = mesh->stride;
	convertConfig->vertex_alignment = NK_ALIGNOF(float);
	convertConfig->global_alpha = 1.0f;
	convertConfig->shape_AA = NK_ANTI_ALIASING_ON;
	convertConfig->line_AA = NK_ANTI_ALIASING_ON;
	convertConfig->circle_segment_count = 22;
	convertConfig->curve_segment_count = 22;
	convertConfig->arc_segment_count = 22;

	// Setup material
	material = std::make_unique<Material>();
	material->vertexShader = Modules::ResourceManager->GetShader("vs_ocornut_imgui");
	material->fragmentShader = Modules::ResourceManager->GetShader("fs_ocornut_imgui");

	// Setup fonts
	fontAtlas = std::make_unique<nk_font_atlas>();
	nk_font_atlas_init(fontAtlas.get(), allocator.get());
	nk_font_atlas_begin(fontAtlas.get());

	// Setup font texture
	int width = 0;
	int height = 0;
	const void* pTextureData = nk_font_atlas_bake(fontAtlas.get(), &width, &height, NK_FONT_ATLAS_RGBA32);
	fontTexture = std::make_shared<Texture>(static_cast<uint16_t>(width), static_cast<uint16_t>(height), Texture::EFormat::RGBA8, pTextureData);
	material->AddTexture(fontTexture);

	nk_font_atlas_end(fontAtlas.get(), nk_handle_ptr(&fontTexture), &convertConfig->null);

	if(fontAtlas->default_font)
	{
		nk_style_set_font(ctx.get(), &fontAtlas->default_font->handle);
	}

	cmdBuffer = std::make_unique<nk_buffer>();
	nk_buffer_init(cmdBuffer.get(), allocator.get(), 4 * 1024);
}

void NuklearUI::Kill()
{
	nk_buffer_free(cmdBuffer.get());
	cmdBuffer = nullptr;

	nk_font_atlas_clear(fontAtlas.get());
	fontAtlas = nullptr;

	nk_free(ctx.get());
	ctx = nullptr;

	fontTexture = nullptr;
	material = nullptr;
	convertConfig = nullptr;
	mesh = nullptr;
}

void NuklearUI::NewFrame(OSWindow* pWindow)
{
	ctx->delta_time_seconds = Modules::Application->GetTimer()->GetSecDelta();

	nk_input_begin(ctx.get());

	// Keyboard
	nk_input_key(ctx.get(), NK_KEY_SHIFT, Modules::Input->IsKeyDown(KEY_LSHIFT, EKeyModifier::DontCare) || Modules::Input->IsKeyDown(KEY_RSHIFT, EKeyModifier::DontCare));
	nk_input_key(ctx.get(), NK_KEY_CTRL, Modules::Input->IsKeyDown(KEY_LCONTROL, EKeyModifier::DontCare) || Modules::Input->IsKeyDown(KEY_RCONTROL, EKeyModifier::DontCare));
	nk_input_key(ctx.get(), NK_KEY_DEL, Modules::Input->IsKeyDown(KEY_DELETE));
	nk_input_key(ctx.get(), NK_KEY_ENTER, Modules::Input->IsKeyDown(KEY_RETURN) || Modules::Input->IsKeyDown(KEY_NUMPADENTER));
	nk_input_key(ctx.get(), NK_KEY_TAB, Modules::Input->IsKeyDown(KEY_TAB));
	nk_input_key(ctx.get(), NK_KEY_BACKSPACE, Modules::Input->IsKeyDown(KEY_BACKSPACE));
	nk_input_key(ctx.get(), NK_KEY_COPY, Modules::Input->IsKeyDown(KEY_C, EKeyModifier::Ctrl));
	nk_input_key(ctx.get(), NK_KEY_CUT, Modules::Input->IsKeyDown(KEY_X, EKeyModifier::Ctrl));
	nk_input_key(ctx.get(), NK_KEY_PASTE, Modules::Input->IsKeyDown(KEY_V, EKeyModifier::Ctrl));
	nk_input_key(ctx.get(), NK_KEY_UP, Modules::Input->IsKeyDown(KEY_UP));
	nk_input_key(ctx.get(), NK_KEY_DOWN, Modules::Input->IsKeyDown(KEY_DOWN));
	nk_input_key(ctx.get(), NK_KEY_LEFT, Modules::Input->IsKeyDown(KEY_LEFT));
	nk_input_key(ctx.get(), NK_KEY_RIGHT, Modules::Input->IsKeyDown(KEY_RIGHT));
//	nk_input_key(ctx.get(), NK_KEY_TEXT_INSERT_MODE, Modules::Input->IsKeyDown());
//	nk_input_key(ctx.get(), NK_KEY_TEXT_REPLACE_MODE, Modules::Input->IsKeyDown());
//	nk_input_key(ctx.get(), NK_KEY_TEXT_RESET_MODE, Modules::Input->IsKeyDown());
//	nk_input_key(ctx.get(), NK_KEY_TEXT_LINE_START, Modules::Input->IsKeyDown(KEY_HOME));
//	nk_input_key(ctx.get(), NK_KEY_TEXT_LINE_END, Modules::Input->IsKeyDown(KEY_END));
	nk_input_key(ctx.get(), NK_KEY_TEXT_START, Modules::Input->IsKeyDown(KEY_HOME));
	nk_input_key(ctx.get(), NK_KEY_TEXT_END, Modules::Input->IsKeyDown(KEY_HOME));
	nk_input_key(ctx.get(), NK_KEY_TEXT_UNDO, Modules::Input->IsKeyDown(KEY_Z, EKeyModifier::Ctrl));
	nk_input_key(ctx.get(), NK_KEY_TEXT_REDO, Modules::Input->IsKeyDown(KEY_Y, EKeyModifier::Ctrl));
	nk_input_key(ctx.get(), NK_KEY_TEXT_SELECT_ALL, Modules::Input->IsKeyDown(KEY_A, EKeyModifier::Ctrl));
	nk_input_key(ctx.get(), NK_KEY_TEXT_WORD_LEFT, Modules::Input->IsKeyDown(KEY_LEFT, EKeyModifier::Ctrl));
	nk_input_key(ctx.get(), NK_KEY_TEXT_WORD_RIGHT, Modules::Input->IsKeyDown(KEY_RIGHT, EKeyModifier::Ctrl));
	nk_input_key(ctx.get(), NK_KEY_SCROLL_START, Modules::Input->IsKeyDown(KEY_HOME, EKeyModifier::Ctrl));
	nk_input_key(ctx.get(), NK_KEY_SCROLL_END, Modules::Input->IsKeyDown(KEY_END, EKeyModifier::Ctrl));
	nk_input_key(ctx.get(), NK_KEY_SCROLL_DOWN, Modules::Input->IsKeyDown(KEY_PGDOWN));
	nk_input_key(ctx.get(), NK_KEY_SCROLL_UP, Modules::Input->IsKeyDown(KEY_PGUP));

	const String& typedCharacters = Modules::Input->GetTypingCharacters();
	for(size_t i = 0; i < typedCharacters.Length(); ++i)
	{
		nk_input_char(ctx.get(), typedCharacters.Str()[i]);
	}

	// Mouse
	const Vector2& mousePos = Modules::Input->GetOsMouseCursorPos();
	const int x = static_cast<int>(mousePos.GetX());
	const int y = static_cast<int>(mousePos.GetY());
	nk_input_motion(ctx.get(), x, y);
	nk_input_button(ctx.get(), NK_BUTTON_LEFT, x, y, Modules::Input->IsMouseButtonDown(Mouse::Button_Left));
	nk_input_button(ctx.get(), NK_BUTTON_MIDDLE, x, y, Modules::Input->IsMouseButtonDown(Mouse::Button_Middle));
	nk_input_button(ctx.get(), NK_BUTTON_RIGHT, x, y, Modules::Input->IsMouseButtonDown(Mouse::Button_Right));

	float mouseWheel = 0.0f;
	float mouseWheelH = 0.0f;
	for(const Mouse& mouse : Modules::Input->GetMouses())
	{
		mouseWheel += mouse.GetAxisDelta(Mouse::Wheel);
		mouseWheelH += mouse.GetAxisDelta(Mouse::Wheel_Horizontal);
	}
	nk_input_scroll(ctx.get(), nk_vec2(mouseWheelH, mouseWheel));

	nk_input_end(ctx.get());
}

void NuklearUI::Render()
{
	Modules::Render->SetDepthWriteEnabled(false);
	Modules::Render->SetCullMode(Render::ECullMode::None);
	Modules::Render->SetBlendMode(Render::EBlend::SrcAlpha, Render::EBlend::InvSrcAlpha, Render::EBlendOp::Add);

	// Update mesh with packed buffers for contiguous indices and vertices
	nk_buffer indexBuffer;
	nk_buffer vertexBuffer;
	nk_buffer_init_fixed(&indexBuffer, mesh->indices.get(), mesh->numIndices);
	nk_buffer_init_fixed(&vertexBuffer, mesh->vertices.get(), mesh->numVertices);
	nk_flags result = nk_convert(ctx.get(), cmdBuffer.get(), &vertexBuffer, &indexBuffer, convertConfig.get());
	if(result != NK_CONVERT_SUCCESS)
	{
		// Skip rendering
		ASSERT(false && "nk_convert failed");
		return;
	}

	mesh->isIndicesDirty = true;
	mesh->isVerticesDirty = true;

	uint32_t indexOffset = 0;
	const nk_draw_command* pCmd = nullptr;
	nk_draw_foreach(pCmd, ctx.get(), cmdBuffer.get())
	{
		if(pCmd->elem_count == 0)
		{
			continue;
		}

		material->ChangeTexture(0, *static_cast<const std::shared_ptr<Texture>*>(pCmd->texture.ptr));

		const uint16_t x = static_cast<uint16_t>(std::max(0.0f, pCmd->clip_rect.x));
		const uint16_t y = static_cast<uint16_t>(std::max(0.0f, pCmd->clip_rect.y));
		const uint16_t w = static_cast<uint16_t>(std::min<float>(pCmd->clip_rect.w, UINT16_MAX));
		const uint16_t h = static_cast<uint16_t>(std::min<float>(pCmd->clip_rect.h, UINT16_MAX));
		Modules::Render->SetScissor(x, y, w, h);

		Modules::Render->RenderMesh(mesh.get(), material.get(), indexOffset, 0, pCmd->elem_count);
		indexOffset += pCmd->elem_count;
	}

	Modules::Render->SetScissor();

	nk_clear(ctx.get());
	nk_buffer_clear(cmdBuffer.get());
}

void NuklearUI::BeginWindow(const String& label, const Vector2& initialPos, const Vector2& initialSize)
{
	nk_flags flags = NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE;
	nk_begin(ctx.get(), label.Str(), nk_rect(initialPos.GetX(), initialPos.GetY(), initialSize.GetX(), initialSize.GetY()), flags);
}

void NuklearUI::EndWindow()
{
	nk_end(ctx.get());
}

void NuklearUI::Text(const String& label)
{
	nk_text(ctx.get(), label.Str(), static_cast<int>(label.Length()), NK_TEXT_LEFT);
}

bool NuklearUI::Button(const String& label, const Vector2& size)
{
	return nk_button_text(ctx.get(), label.Str(), static_cast<int>(label.Length()));
}

bool NuklearUI::Checkbox(const String& label, bool& isChecked)
{
	int active = isChecked ? 1 : 0;
	if(nk_checkbox_text(ctx.get(), label.Str(), static_cast<int>(label.Length()), &active))
	{
		isChecked = (active != 0);
		return true;
	}

	return false;
}

bool NuklearUI::RadioButtonOption(const String& label, bool isActive)
{
	int active = isActive ? 1 : 0;
	return nk_radio_text(ctx.get(), label.Str(), static_cast<int>(label.Length()), &active) && active;
}

bool NuklearUI::InputField(const String& label, float& value)
{
	return false;
}

bool NuklearUI::InputField(const String& label, Vector3& value)
{
	float elements[3];
	value.StoreXYZ(elements);

	return false;
}

bool NuklearUI::Slider(const String& label, int32_t& value, int32_t minValue, int32_t maxValue)
{
	return nk_slider_int(ctx.get(), minValue, &value, maxValue, 1);
}

bool NuklearUI::Slider(const String& label, float& value, float minValue, float maxValue)
{
	return nk_slider_float(ctx.get(), minValue, &value, maxValue, 0.1f);
}

bool NuklearUI::ColorPicker(const String& label, float(&colorRGB)[3])
{
	nk_colorf nkColor;
	memcpy(&nkColor, colorRGB, 3 * sizeof(float));
	int rv = nk_color_pick(ctx.get(), &nkColor, NK_RGB);
	memcpy(colorRGB, &nkColor, 3 * sizeof(float));
	return (rv != 0);
}

bool NuklearUI::ColorPicker(const String& label, float(&colorRGBA)[4])
{
	return nk_color_pick(ctx.get(), reinterpret_cast<nk_colorf*>(colorRGBA), NK_RGBA);
}

void NuklearUI::SameLine()
{
}
