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
#include "Engine/Render/Mesh.h"
#include "Engine/Render/Render.h"
#include "Engine/Render/Shader.h"
#include "Engine/Render/Texture.h"

#include "Engine/Resource/ResourceManager.h"

constexpr int kDefaultRowHeight = 25;

NuklearUI::NuklearUI()
	: allocator(std::make_unique<nk_allocator>())
{
	allocator->alloc = &NuklearCallbacks::MallocWrapper;
	allocator->free = &NuklearCallbacks::FreeWrapper;
	allocator->userdata = nk_handle_ptr(nullptr);
}

NuklearUI::~NuklearUI()
{
}

void NuklearUI::Init()
{
	ctx = std::make_unique<nk_context>();
	nk_init(ctx.get(), allocator.get(), nullptr);
	ctx->clip.copy = &NuklearCallbacks::ClipboardCopy;
	ctx->clip.paste = &NuklearCallbacks::ClipboardPaste;
	ctx->clip.userdata = nk_handle_ptr(nullptr);
	ctx->button_behavior = NK_BUTTON_REPEATER;

	// Dynamic mesh for the draw list
	const std::initializer_list<Mesh::VertexLayout> vertexLayout =
	{
		{ Mesh::EAttrib::Position,	2, Mesh::EAttribType::Float },
		{ Mesh::EAttrib::Texcoord0,	2, Mesh::EAttribType::Float },
		{ Mesh::EAttrib::Color,		4, Mesh::EAttribType::Uint8 }
	};
	mesh = std::make_unique<DynamicMesh>(vertexLayout, 128 * 1024, 256 * 1024);
	static_assert(sizeof(nk_draw_index) == sizeof(uint16_t), "Conversion is required for index buffer");

	static constexpr nk_draw_vertex_layout_element s_nkVertexLayout[] =
	{
		{ NK_VERTEX_POSITION, NK_FORMAT_FLOAT, 0 },
		{ NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, 2 * sizeof(float) },
		{ NK_VERTEX_COLOR, NK_FORMAT_R8G8B8A8, 4 * sizeof(float) },
		{ NK_VERTEX_LAYOUT_END }
	};

	convertConfig = std::make_unique<nk_convert_config>();
	convertConfig->vertex_layout = s_nkVertexLayout;
	convertConfig->vertex_size = mesh->GetVertexSize();
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

void NuklearUI::NewFrame(OSWindow& window)
{
	ctx->clip.userdata = nk_handle_ptr(&window);
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

	const float mouseWheel = Modules::Input->GetMouseAxisDelta(Mouse::Wheel);
	const float mouseWheelH = Modules::Input->GetMouseAxisDelta(Mouse::Wheel_Horizontal);
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
	nk_buffer_init_fixed(&indexBuffer, mesh->indices.get(), mesh->GetNumIndices());
	nk_buffer_init_fixed(&vertexBuffer, mesh->vertices.get(), mesh->GetNumVertices());
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

bool NuklearUI::BeginWindow(const String& label, const Vector2& initialPos, const Vector2& initialSize, bool* pOpen, EWindowFlags flags)
{
	nk_flags nkFlags = NK_WINDOW_BORDER;
	if(!(flags & WindowFlags_NoTitleBar))
	{
		nkFlags |= NK_WINDOW_TITLE;
	}
	if(!(flags & WindowFlags_NoResize))
	{
		nkFlags |= NK_WINDOW_SCALABLE;
	}
	if(!(flags & WindowFlags_NoMove))
	{
		nkFlags |= NK_WINDOW_MOVABLE;
	}
	if(flags & WindowFlags_NoScrollbar)
	{
		nkFlags |= NK_WINDOW_NO_SCROLLBAR;
	}

	if(pOpen != nullptr)
	{
		nkFlags |= NK_WINDOW_CLOSABLE;

		nk_window_show(ctx.get(), label.Str(), *pOpen ? NK_SHOWN : NK_HIDDEN);
	}

	const bool isVisible = nk_begin(ctx.get(), label.Str(), nk_rect(initialPos.GetX(), initialPos.GetY(), initialSize.GetX(), initialSize.GetY()), nkFlags);
	if(isVisible)
	{
		nk_layout_row_dynamic(ctx.get(), kDefaultRowHeight, 1);
	}

	if(pOpen != nullptr)
	{
		*pOpen = isVisible;
	}

	return isVisible;
}

void NuklearUI::EndWindow()
{
	nk_end(ctx.get());
}

void NuklearUI::Text(const String& label)
{
	nk_text(ctx.get(), label.Str(), static_cast<int>(label.Length()), NK_TEXT_LEFT);
}

bool NuklearUI::TextInput(WritableString& value)
{
	constexpr int kMaxSize = 255;
	StackString<kMaxSize> string = value;
	int len = static_cast<int>(value.Length());
	const nk_flags result = nk_edit_string(ctx.get(), NK_EDIT_FIELD | NK_EDIT_AUTO_SELECT, string.AsCharBufferWithSize(kMaxSize - 1), &len, kMaxSize, nk_filter_default);
	string.TruncateAt(len);
	if(result == NK_EDIT_ACTIVE && !value.Equals(string))
	{
		value = string;
		return true;
	}

	return false;
}

bool NuklearUI::Button(const String& label, const Vector2& size)
{
	nk_button_set_behavior(ctx.get(), NK_BUTTON_DEFAULT);
	return nk_button_text(ctx.get(), label.Str(), static_cast<int>(label.Length()));
}

bool NuklearUI::ArrowButton(EArrowDir dir)
{
	int isPressed = nk_false;
	switch(dir)
	{
		case EArrowDir::Left:	isPressed = nk_button_symbol(ctx.get(), NK_SYMBOL_TRIANGLE_LEFT); break;
		case EArrowDir::Right:	isPressed = nk_button_symbol(ctx.get(), NK_SYMBOL_TRIANGLE_RIGHT); break;
		case EArrowDir::Up:		isPressed = nk_button_symbol(ctx.get(), NK_SYMBOL_TRIANGLE_UP); break;
		case EArrowDir::Down:	isPressed = nk_button_symbol(ctx.get(), NK_SYMBOL_TRIANGLE_DOWN); break;
	}

	return (isPressed == nk_true);
}

bool NuklearUI::Checkbox(bool& isChecked, const String& label)
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

void NuklearUI::ProgressBar(float progressPercent)
{
	nk_size progress = static_cast<nk_size>(progressPercent * 1000.0f);
	nk_progress(ctx.get(), &progress, 1000, NK_FIXED);
}

bool NuklearUI::ValueSpinner(int32_t& value, int32_t minValue, int32_t maxValue, float speed)
{
	int32_t oldValue = value;
	nk_property_int(ctx.get(), "#", minValue, &value, maxValue, 1, std::max(1.0f, speed));
	return (oldValue != value);
}

bool NuklearUI::ValueSpinner(float& value, float minValue, float maxValue, float speed)
{
	float oldValue = value;
	nk_property_float(ctx.get(), "#", minValue, &value, maxValue, speed, speed);
	return (oldValue != value);
}

bool NuklearUI::ValueEdit(float& value)
{
	return ValueSpinner(value);
}

bool NuklearUI::ValueEdit(Vector3& value)
{
	bool isValueChanged = false;

	nk_style_push_vec2(ctx.get(), &ctx->style.window.group_padding, nk_vec2(0.0f, 0.0f));

	if(nk_group_begin(ctx.get(), "#", NK_WINDOW_NO_SCROLLBAR))
	{
		float elements[3];
		value.StoreXYZ(elements);

		nk_layout_row_dynamic(ctx.get(), kDefaultRowHeight, 3);
		isValueChanged |= ValueSpinner(elements[0]);
		isValueChanged |= ValueSpinner(elements[1]);
		isValueChanged |= ValueSpinner(elements[2]);
		if(isValueChanged)
		{
			value.LoadXYZ(elements);
		}

		nk_group_end(ctx.get());
	}

	nk_style_pop_vec2(ctx.get());

	return isValueChanged;
}

bool NuklearUI::Slider(int32_t& value, int32_t minValue, int32_t maxValue)
{
	return nk_slider_int(ctx.get(), minValue, &value, maxValue, 1);
}

bool NuklearUI::Slider(float& value, float minValue, float maxValue)
{
	const struct nk_rect bounds = nk_layout_widget_bounds(ctx.get());
	const float step = (maxValue - minValue) / bounds.w;
	return nk_slider_float(ctx.get(), minValue, &value, maxValue, step);
}

bool NuklearUI::ColorPicker(float(&colorRGB)[3])
{
	bool result = false;

	nk_color color = nk_rgb_fv(colorRGB);
	if(nk_combo_begin_color(ctx.get(), color, nk_vec2(200, 400)))
	{
		nk_layout_row_dynamic(ctx.get(), 120, 1);

		nk_colorf colorf;
		memcpy(&colorf, colorRGB, 3 * sizeof(float));
		if(nk_color_pick(ctx.get(), &colorf, NK_RGB) != 0)
		{
			result = true;
			memcpy(colorRGB, &colorf, 3 * sizeof(float));
			color = nk_rgb_fv(colorRGB);
		}

		nk_layout_row_dynamic(ctx.get(), kDefaultRowHeight, 1);
		const nk_color origColor = color;
		color.r = (nk_byte)nk_propertyi(ctx.get(), "#R", 0, color.r, 255, 1, 1);
		color.g = (nk_byte)nk_propertyi(ctx.get(), "#G", 0, color.g, 255, 1, 1);
		color.b = (nk_byte)nk_propertyi(ctx.get(), "#B", 0, color.b, 255, 1, 1);
		if(memcmp(&color, &origColor, sizeof(nk_color)) != 0)
		{
			colorf = nk_color_cf(color);
			memcpy(colorRGB, &colorf, 3 * sizeof(float));
			result = true;
		}

		nk_combo_end(ctx.get());
	}

	return result;
}

bool NuklearUI::ColorPicker(float(&colorRGBA)[4])
{
	bool result = false;

	nk_color color = nk_rgba_fv(colorRGBA);
	if(nk_combo_begin_color(ctx.get(), color, nk_vec2(200, 400)))
	{
		nk_layout_row_dynamic(ctx.get(), 120, 1);

		result |= (nk_color_pick(ctx.get(), reinterpret_cast<nk_colorf*>(colorRGBA), NK_RGBA) != 0);

		nk_layout_row_dynamic(ctx.get(), kDefaultRowHeight, 1);
		const nk_color origColor = color;
		color.r = (nk_byte)nk_propertyi(ctx.get(), "#R", 0, color.r, 255, 1, 1);
		color.g = (nk_byte)nk_propertyi(ctx.get(), "#G", 0, color.g, 255, 1, 1);
		color.b = (nk_byte)nk_propertyi(ctx.get(), "#B", 0, color.b, 255, 1, 1);
		color.a = (nk_byte)nk_propertyi(ctx.get(), "#A", 0, color.a, 255, 1, 1);
		if(memcmp(&color, &origColor, sizeof(nk_color)) != 0)
		{
			nk_color_fv(colorRGBA, color);
			result = true;
		}

		nk_combo_end(ctx.get());
	}

	return result;
}

void NuklearUI::LayoutColumns(uint8_t numColumns, const float* pRatio)
{
	if(pRatio != nullptr)
	{
		nk_layout_row(ctx.get(), NK_DYNAMIC, kDefaultRowHeight, numColumns, pRatio);
	}
	else
	{
		nk_layout_row_dynamic(ctx.get(), kDefaultRowHeight, numColumns);
	}
}
