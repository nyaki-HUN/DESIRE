#include "stdafx_Nuklear.h"
#include "NuklearUI.h"

#include "NuklearCallbacks.h"

#include "Engine/Application/Application.h"
#include "Engine/Application/OSWindow.h"
#include "Engine/Application/ResourceManager.h"

#include "Engine/Core/Math/Vector3.h"
#include "Engine/Core/Memory/MemorySystem.h"
#include "Engine/Core/String/String.h"
#include "Engine/Core/Timer.h"

#include "Engine/Input/Input.h"

#include "Engine/Render/Material.h"
#include "Engine/Render/Mesh.h"
#include "Engine/Render/Render.h"
#include "Engine/Render/Renderable.h"
#include "Engine/Render/Shader.h"
#include "Engine/Render/Texture.h"

constexpr float kDefaultRowHeight = 0.0f;

static constexpr nk_draw_vertex_layout_element s_nkVertexLayout[] =
{
	{ NK_VERTEX_POSITION, NK_FORMAT_FLOAT, 0 },
	{ NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, 2 * sizeof(float) },
	{ NK_VERTEX_COLOR, NK_FORMAT_R8G8B8A8, 4 * sizeof(float) },
	{ NK_VERTEX_LAYOUT_END }
};

NuklearUI::NuklearUI()
	: m_spAllocator(std::make_unique<nk_allocator>())
{
	m_spAllocator->alloc = &NuklearCallbacks::MallocWrapper;
	m_spAllocator->free = &NuklearCallbacks::FreeWrapper;
	m_spAllocator->userdata = nk_handle_ptr(nullptr);
}

NuklearUI::~NuklearUI()
{
}

void NuklearUI::Init()
{
	m_spContext = std::make_unique<nk_context>();
	nk_init(m_spContext.get(), m_spAllocator.get(), nullptr);
	m_spContext->clip.copy = &NuklearCallbacks::ClipboardCopy;
	m_spContext->clip.paste = &NuklearCallbacks::ClipboardPaste;
	m_spContext->clip.userdata = nk_handle_ptr(nullptr);
	m_spContext->button_behavior = NK_BUTTON_REPEATER;

	m_spRenderable = std::make_unique<Renderable>();

	// Dynamic mesh for the draw list
	Array<Mesh::VertexLayout> vertexLayout =
	{
		{ Mesh::EAttrib::Position,	2, Mesh::EAttribType::Float },
		{ Mesh::EAttrib::Texcoord0,	2, Mesh::EAttribType::Float },
		{ Mesh::EAttrib::Color,		4, Mesh::EAttribType::Uint8 }
	};
	m_spRenderable->m_spMesh = std::make_shared<DynamicMesh>(std::move(vertexLayout));
	m_spRenderable->m_spMesh->SetNumIndices(128 * 1024);
	m_spRenderable->m_spMesh->SetNumVertices(256 * 1024);
	static_assert(sizeof(nk_draw_index) == sizeof(uint16_t), "Conversion is required for index buffer");

	m_spConvertConfig = std::make_unique<nk_convert_config>();
	m_spConvertConfig->vertex_layout = s_nkVertexLayout;
	m_spConvertConfig->vertex_size = m_spRenderable->m_spMesh->GetVertexSize();
	m_spConvertConfig->vertex_alignment = NK_ALIGNOF(float);
	m_spConvertConfig->global_alpha = 1.0f;
	m_spConvertConfig->shape_AA = NK_ANTI_ALIASING_ON;
	m_spConvertConfig->line_AA = NK_ANTI_ALIASING_ON;
	m_spConvertConfig->circle_segment_count = 22;
	m_spConvertConfig->curve_segment_count = 22;
	m_spConvertConfig->arc_segment_count = 22;

	// Setup material
	m_spRenderable->m_spMaterial = std::make_unique<Material>();
	m_spRenderable->m_spMaterial->m_spVertexShader = Modules::Application->GetResourceManager().GetShader("vs_ocornut_imgui");
	m_spRenderable->m_spMaterial->m_spPixelShader = Modules::Application->GetResourceManager().GetShader("fs_ocornut_imgui");
	m_spRenderable->m_spMaterial->m_cullMode = ECullMode::None;
	m_spRenderable->m_spMaterial->m_isBlendEnabled = true;
	m_spRenderable->m_spMaterial->m_isDepthWriteEnabled = false;

	// Setup fonts
	m_spFontAtlas = std::make_unique<nk_font_atlas>();
	nk_font_atlas_init(m_spFontAtlas.get(), m_spAllocator.get());
	nk_font_atlas_begin(m_spFontAtlas.get());

	// Setup font texture
	int32_t width = 0;
	int32_t height = 0;
	const void* pTextureData = nk_font_atlas_bake(m_spFontAtlas.get(), &width, &height, NK_FONT_ATLAS_RGBA32);
	m_spFontTexture = std::make_shared<Texture>(static_cast<uint16_t>(width), static_cast<uint16_t>(height), Texture::EFormat::RGBA8, pTextureData);
	m_spRenderable->m_spMaterial->AddTexture(m_spFontTexture);

	nk_font_atlas_end(m_spFontAtlas.get(), nk_handle_ptr(&m_spFontTexture), &m_spConvertConfig->null);

	if(m_spFontAtlas->default_font)
	{
		nk_style_set_font(m_spContext.get(), &m_spFontAtlas->default_font->handle);
	}

	m_spCmdBuffer = std::make_unique<nk_buffer>();
	nk_buffer_init(m_spCmdBuffer.get(), m_spAllocator.get(), 4 * 1024);
}

void NuklearUI::Kill()
{
	nk_buffer_free(m_spCmdBuffer.get());
	m_spCmdBuffer = nullptr;

	nk_font_atlas_clear(m_spFontAtlas.get());
	m_spFontAtlas = nullptr;

	m_spFontTexture = nullptr;
	m_spConvertConfig = nullptr;
	m_spRenderable = nullptr;

	nk_free(m_spContext.get());
	m_spContext = nullptr;
}

void NuklearUI::NewFrame(OSWindow& window)
{
	m_spContext->clip.userdata = nk_handle_ptr(&window);
	m_spContext->delta_time_seconds = Modules::Application->GetTimer()->GetSecDelta();

	nk_input_begin(m_spContext.get());

	// Keyboard
	nk_input_key(m_spContext.get(), NK_KEY_SHIFT, Modules::Input->IsKeyDown(KEY_LSHIFT, EKeyModifier::DontCare) || Modules::Input->IsKeyDown(KEY_RSHIFT, EKeyModifier::DontCare));
	nk_input_key(m_spContext.get(), NK_KEY_CTRL, Modules::Input->IsKeyDown(KEY_LCONTROL, EKeyModifier::DontCare) || Modules::Input->IsKeyDown(KEY_RCONTROL, EKeyModifier::DontCare));
	nk_input_key(m_spContext.get(), NK_KEY_DEL, Modules::Input->IsKeyDown(KEY_DELETE));
	nk_input_key(m_spContext.get(), NK_KEY_ENTER, Modules::Input->IsKeyDown(KEY_RETURN) || Modules::Input->IsKeyDown(KEY_NUMPADENTER));
	nk_input_key(m_spContext.get(), NK_KEY_TAB, Modules::Input->IsKeyDown(KEY_TAB));
	nk_input_key(m_spContext.get(), NK_KEY_BACKSPACE, Modules::Input->IsKeyDown(KEY_BACKSPACE));
	nk_input_key(m_spContext.get(), NK_KEY_COPY, Modules::Input->IsKeyDown(KEY_C, EKeyModifier::Ctrl));
	nk_input_key(m_spContext.get(), NK_KEY_CUT, Modules::Input->IsKeyDown(KEY_X, EKeyModifier::Ctrl));
	nk_input_key(m_spContext.get(), NK_KEY_PASTE, Modules::Input->IsKeyDown(KEY_V, EKeyModifier::Ctrl));
	nk_input_key(m_spContext.get(), NK_KEY_UP, Modules::Input->IsKeyDown(KEY_UP));
	nk_input_key(m_spContext.get(), NK_KEY_DOWN, Modules::Input->IsKeyDown(KEY_DOWN));
	nk_input_key(m_spContext.get(), NK_KEY_LEFT, Modules::Input->IsKeyDown(KEY_LEFT));
	nk_input_key(m_spContext.get(), NK_KEY_RIGHT, Modules::Input->IsKeyDown(KEY_RIGHT));
//	nk_input_key(m_spContext.get(), NK_KEY_TEXT_INSERT_MODE, Modules::Input->IsKeyDown());
//	nk_input_key(m_spContext.get(), NK_KEY_TEXT_REPLACE_MODE, Modules::Input->IsKeyDown());
//	nk_input_key(m_spContext.get(), NK_KEY_TEXT_RESET_MODE, Modules::Input->IsKeyDown());
//	nk_input_key(m_spContext.get(), NK_KEY_TEXT_LINE_START, Modules::Input->IsKeyDown(KEY_HOME));
//	nk_input_key(m_spContext.get(), NK_KEY_TEXT_LINE_END, Modules::Input->IsKeyDown(KEY_END));
	nk_input_key(m_spContext.get(), NK_KEY_TEXT_START, Modules::Input->IsKeyDown(KEY_HOME));
	nk_input_key(m_spContext.get(), NK_KEY_TEXT_END, Modules::Input->IsKeyDown(KEY_HOME));
	nk_input_key(m_spContext.get(), NK_KEY_TEXT_UNDO, Modules::Input->IsKeyDown(KEY_Z, EKeyModifier::Ctrl));
	nk_input_key(m_spContext.get(), NK_KEY_TEXT_REDO, Modules::Input->IsKeyDown(KEY_Y, EKeyModifier::Ctrl));
	nk_input_key(m_spContext.get(), NK_KEY_TEXT_SELECT_ALL, Modules::Input->IsKeyDown(KEY_A, EKeyModifier::Ctrl));
	nk_input_key(m_spContext.get(), NK_KEY_TEXT_WORD_LEFT, Modules::Input->IsKeyDown(KEY_LEFT, EKeyModifier::Ctrl));
	nk_input_key(m_spContext.get(), NK_KEY_TEXT_WORD_RIGHT, Modules::Input->IsKeyDown(KEY_RIGHT, EKeyModifier::Ctrl));
	nk_input_key(m_spContext.get(), NK_KEY_SCROLL_START, Modules::Input->IsKeyDown(KEY_HOME, EKeyModifier::Ctrl));
	nk_input_key(m_spContext.get(), NK_KEY_SCROLL_END, Modules::Input->IsKeyDown(KEY_END, EKeyModifier::Ctrl));
	nk_input_key(m_spContext.get(), NK_KEY_SCROLL_DOWN, Modules::Input->IsKeyDown(KEY_PGDOWN));
	nk_input_key(m_spContext.get(), NK_KEY_SCROLL_UP, Modules::Input->IsKeyDown(KEY_PGUP));

	const String& typedCharacters = Modules::Input->GetTypingCharacters();
	for(size_t i = 0; i < typedCharacters.Length(); ++i)
	{
		nk_input_char(m_spContext.get(), typedCharacters.Str()[i]);
	}

	// Mouse
	const Vector2& mousePos = Modules::Input->GetOsMouseCursorPos();
	const int32_t x = static_cast<int>(mousePos.GetX());
	const int32_t y = static_cast<int>(mousePos.GetY());
	nk_input_motion(m_spContext.get(), x, y);
	nk_input_button(m_spContext.get(), NK_BUTTON_LEFT, x, y, Modules::Input->IsMouseButtonDown(Mouse::Button_Left));
	nk_input_button(m_spContext.get(), NK_BUTTON_MIDDLE, x, y, Modules::Input->IsMouseButtonDown(Mouse::Button_Middle));
	nk_input_button(m_spContext.get(), NK_BUTTON_RIGHT, x, y, Modules::Input->IsMouseButtonDown(Mouse::Button_Right));

	const float mouseWheel = Modules::Input->GetMouseAxisDelta(Mouse::Wheel);
	const float mouseWheelH = Modules::Input->GetMouseAxisDelta(Mouse::Wheel_Horizontal);
	nk_input_scroll(m_spContext.get(), nk_vec2(mouseWheelH, mouseWheel));

	nk_input_end(m_spContext.get());
}

void NuklearUI::Render()
{
	// Update mesh with packed buffers for contiguous indices and vertices
	nk_buffer indexBuffer;
	nk_buffer vertexBuffer;
	nk_buffer_init_fixed(&indexBuffer, m_spRenderable->m_spMesh->m_spIndices.get(), m_spRenderable->m_spMesh->GetNumIndices());
	nk_buffer_init_fixed(&vertexBuffer, m_spRenderable->m_spMesh->m_spVertices.get(), m_spRenderable->m_spMesh->GetNumVertices());
	nk_flags result = nk_convert(m_spContext.get(), m_spCmdBuffer.get(), &vertexBuffer, &indexBuffer, m_spConvertConfig.get());
	if(result != NK_CONVERT_SUCCESS)
	{
		// Skip rendering
		ASSERT(false && "nk_convert failed");
		return;
	}

	static_cast<DynamicMesh*>(m_spRenderable->m_spMesh.get())->m_isIndicesDirty = true;
	static_cast<DynamicMesh*>(m_spRenderable->m_spMesh.get())->m_isVerticesDirty = true;

	uint32_t indexOffset = 0;
	const nk_draw_command* pCmd = nullptr;
	nk_draw_foreach(pCmd, m_spContext.get(), m_spCmdBuffer.get())
	{
		if(pCmd->elem_count == 0)
		{
			continue;
		}

		m_spRenderable->m_spMaterial->ChangeTexture(0, *static_cast<const std::shared_ptr<Texture>*>(pCmd->texture.ptr));

		const uint16_t x = static_cast<uint16_t>(std::max(0.0f, pCmd->clip_rect.x));
		const uint16_t y = static_cast<uint16_t>(std::max(0.0f, pCmd->clip_rect.y));
		const uint16_t w = static_cast<uint16_t>(std::min<float>(pCmd->clip_rect.w, UINT16_MAX));
		const uint16_t h = static_cast<uint16_t>(std::min<float>(pCmd->clip_rect.h, UINT16_MAX));
		Modules::Render->SetScissor(x, y, w, h);

		Modules::Render->RenderRenderable(*m_spRenderable, indexOffset, 0, pCmd->elem_count);
		indexOffset += pCmd->elem_count;
	}

	Modules::Render->SetScissor();

	nk_clear(m_spContext.get());
	nk_buffer_clear(m_spCmdBuffer.get());
}

bool NuklearUI::BeginWindow(const String& label, const Vector2& initialPos, const Vector2& initialSize, bool* pOpen, EWindowFlags flags)
{
	nk_flags nkFlags = NK_WINDOW_BORDER;
	bool hasMenuBar = false;
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
	if(flags & WindowFlags_MenuBar)
	{
		hasMenuBar = true;
	}

	if(pOpen)
	{
		nkFlags |= NK_WINDOW_CLOSABLE;

		nk_window_show(m_spContext.get(), label.Str(), (*pOpen) ? NK_SHOWN : NK_HIDDEN);
	}

	const nk_bool isVisible = nk_begin(m_spContext.get(), label.Str(), nk_rect(initialPos.GetX(), initialPos.GetY(), initialSize.GetX(), initialSize.GetY()), nkFlags);
	if(isVisible && !hasMenuBar)
	{
		SetDefaultLayout();
	}

	if(pOpen)
	{
		*pOpen = isVisible;
	}

	return isVisible;
}

void NuklearUI::EndWindow()
{
	nk_end(m_spContext.get());
}

bool NuklearUI::BeginMenuBar()
{
	nk_menubar_begin(m_spContext.get());
	SetDefaultLayout();
	return true;
}

void NuklearUI::EndMenuBar()
{
	nk_menubar_end(m_spContext.get());
	SetDefaultLayout();
}

bool NuklearUI::BeginMenu(const String& label)
{
	const bool isVisible = nk_menu_begin_text(m_spContext.get(), label.Str(), static_cast<int>(label.Length()), NK_TEXT_LEFT, nk_vec2(100.0, 200.0f));
	if(isVisible)
	{
		SetDefaultLayout();
	}

	return isVisible;
}

void NuklearUI::EndMenu()
{
	nk_menu_end(m_spContext.get());
}

bool NuklearUI::MenuItem(const String& label)
{
	return nk_menu_item_text(m_spContext.get(), label.Str(), static_cast<int>(label.Length()), NK_TEXT_LEFT);
}

bool NuklearUI::BeginTable(const String& id, uint8_t numColumns, const float* pInitialColumnsRatio)
{
	const nk_panel* pPanel = nk_window_get_panel(m_spContext.get());
	if(pPanel && pPanel->row.index < pPanel->row.columns)
	{
		nk_style_push_vec2(m_spContext.get(), &m_spContext->style.window.group_padding, nk_vec2(0.0f, 0.0f));
		const nk_bool isVisible = nk_group_begin(m_spContext.get(), id.Str(), NK_WINDOW_NO_SCROLLBAR);
		nk_style_pop_vec2(m_spContext.get());

		if(!isVisible)
		{
			return false;
		}
	}

	if(pInitialColumnsRatio)
	{
		nk_layout_row(m_spContext.get(), NK_DYNAMIC, kDefaultRowHeight, numColumns, pInitialColumnsRatio);
	}
	else
	{
		nk_layout_row_dynamic(m_spContext.get(), kDefaultRowHeight, numColumns);
	}

	return true;
}

void NuklearUI::EndTable()
{
	const nk_panel* pPanel = nk_window_get_panel(m_spContext.get());
	if(pPanel && pPanel->parent != nullptr)
	{
		nk_group_end(m_spContext.get());
	}
	else
	{
		nk_layout_row_dynamic(m_spContext.get(), kDefaultRowHeight, 1);
	}
}

void NuklearUI::Text(const String& label)
{
	nk_text(m_spContext.get(), label.Str(), static_cast<int>(label.Length()), NK_TEXT_LEFT);
}

bool NuklearUI::TextInput(WritableString& value)
{
	constexpr int32_t kMaxSize = 255;
	StackString<kMaxSize> string = value;
	int32_t len = static_cast<int>(value.Length());
	const nk_flags result = nk_edit_string(m_spContext.get(), NK_EDIT_FIELD | NK_EDIT_AUTO_SELECT, string.AsCharBufferWithSize(kMaxSize - 1), &len, kMaxSize, nk_filter_default);
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
	nk_button_set_behavior(m_spContext.get(), NK_BUTTON_DEFAULT);
	return nk_button_text(m_spContext.get(), label.Str(), static_cast<int>(label.Length()));
}

bool NuklearUI::ArrowButton(EArrowDir dir)
{
	nk_bool isPressed = nk_false;
	switch(dir)
	{
		case EArrowDir::Left:	isPressed = nk_button_symbol(m_spContext.get(), NK_SYMBOL_TRIANGLE_LEFT); break;
		case EArrowDir::Right:	isPressed = nk_button_symbol(m_spContext.get(), NK_SYMBOL_TRIANGLE_RIGHT); break;
		case EArrowDir::Up:		isPressed = nk_button_symbol(m_spContext.get(), NK_SYMBOL_TRIANGLE_UP); break;
		case EArrowDir::Down:	isPressed = nk_button_symbol(m_spContext.get(), NK_SYMBOL_TRIANGLE_DOWN); break;
	}

	return (isPressed == nk_true);
}

bool NuklearUI::Checkbox(bool& isChecked, const String& label)
{
	nk_bool active = isChecked ? nk_true : nk_false;
	if(nk_checkbox_text(m_spContext.get(), label.Str(), static_cast<int>(label.Length()), &active))
	{
		isChecked = (active != 0);
		return true;
	}

	return false;
}

bool NuklearUI::RadioButtonOption(const String& label, bool isActive)
{
	const bool wasActive = isActive;

	nk_bool active = isActive ? nk_true : nk_false;
	const nk_bool isPressed = nk_radio_text(m_spContext.get(), label.Str(), static_cast<int>(label.Length()), &active);

	return isPressed && !wasActive;
}

void NuklearUI::ProgressBar(float progressPercent)
{
	nk_size progress = static_cast<nk_size>(progressPercent * 1000.0f);
	nk_progress(m_spContext.get(), &progress, 1000, NK_FIXED);
}

bool NuklearUI::ValueSpinner(int32_t& value, int32_t minValue, int32_t maxValue, float speed)
{
	int32_t oldValue = value;
	nk_property_int(m_spContext.get(), "#", minValue, &value, maxValue, 1, std::max(1.0f, speed));
	return (oldValue != value);
}

bool NuklearUI::ValueSpinner(float& value, float minValue, float maxValue, float speed)
{
	float oldValue = value;
	nk_property_float(m_spContext.get(), "#", minValue, &value, maxValue, speed, speed);
	return (oldValue != value);
}

bool NuklearUI::ValueEdit(float& value)
{
	return ValueSpinner(value);
}

bool NuklearUI::ValueEdit(Vector3& value)
{
	bool isValueChanged = false;

	nk_style_push_vec2(m_spContext.get(), &m_spContext->style.window.group_padding, nk_vec2(0.0f, 0.0f));

	if(nk_group_begin(m_spContext.get(), "#", NK_WINDOW_NO_SCROLLBAR))
	{
		float elements[3];
		value.StoreXYZ(elements);

		nk_layout_row_dynamic(m_spContext.get(), kDefaultRowHeight, 3);
		isValueChanged |= ValueSpinner(elements[0]);
		isValueChanged |= ValueSpinner(elements[1]);
		isValueChanged |= ValueSpinner(elements[2]);
		if(isValueChanged)
		{
			value.LoadXYZ(elements);
		}

		nk_group_end(m_spContext.get());
	}

	nk_style_pop_vec2(m_spContext.get());

	return isValueChanged;
}

bool NuklearUI::Slider(int32_t& value, int32_t minValue, int32_t maxValue)
{
	return nk_slider_int(m_spContext.get(), minValue, &value, maxValue, 1);
}

bool NuklearUI::Slider(float& value, float minValue, float maxValue)
{
	const struct nk_rect bounds = nk_layout_widget_bounds(m_spContext.get());
	const float step = (maxValue - minValue) / bounds.w;
	return nk_slider_float(m_spContext.get(), minValue, &value, maxValue, step);
}

bool NuklearUI::ColorPicker(float(&colorRGB)[3])
{
	bool result = false;

	nk_color color = nk_rgb_fv(colorRGB);
	if(nk_combo_begin_color(m_spContext.get(), color, nk_vec2(200, 400)))
	{
		nk_layout_row_dynamic(m_spContext.get(), 120, 1);

		nk_colorf colorf;
		memcpy(&colorf, colorRGB, 3 * sizeof(float));
		if(nk_color_pick(m_spContext.get(), &colorf, NK_RGB) != 0)
		{
			result = true;
			memcpy(colorRGB, &colorf, 3 * sizeof(float));
			color = nk_rgb_fv(colorRGB);
		}

		nk_layout_row_dynamic(m_spContext.get(), kDefaultRowHeight, 1);
		const nk_color origColor = color;
		color.r = (nk_byte)nk_propertyi(m_spContext.get(), "#R", 0, color.r, 255, 1, 1);
		color.g = (nk_byte)nk_propertyi(m_spContext.get(), "#G", 0, color.g, 255, 1, 1);
		color.b = (nk_byte)nk_propertyi(m_spContext.get(), "#B", 0, color.b, 255, 1, 1);
		if(memcmp(&color, &origColor, sizeof(nk_color)) != 0)
		{
			colorf = nk_color_cf(color);
			memcpy(colorRGB, &colorf, 3 * sizeof(float));
			result = true;
		}

		nk_combo_end(m_spContext.get());
	}

	return result;
}

bool NuklearUI::ColorPicker(float(&colorRGBA)[4])
{
	bool result = false;

	nk_color color = nk_rgba_fv(colorRGBA);
	if(nk_combo_begin_color(m_spContext.get(), color, nk_vec2(200, 400)))
	{
		nk_layout_row_dynamic(m_spContext.get(), 120, 1);

		result |= (nk_color_pick(m_spContext.get(), reinterpret_cast<nk_colorf*>(colorRGBA), NK_RGBA) != 0);

		nk_layout_row_dynamic(m_spContext.get(), kDefaultRowHeight, 1);
		const nk_color origColor = color;
		color.r = (nk_byte)nk_propertyi(m_spContext.get(), "#R", 0, color.r, 255, 1, 1);
		color.g = (nk_byte)nk_propertyi(m_spContext.get(), "#G", 0, color.g, 255, 1, 1);
		color.b = (nk_byte)nk_propertyi(m_spContext.get(), "#B", 0, color.b, 255, 1, 1);
		color.a = (nk_byte)nk_propertyi(m_spContext.get(), "#A", 0, color.a, 255, 1, 1);
		if(memcmp(&color, &origColor, sizeof(nk_color)) != 0)
		{
			nk_color_fv(colorRGBA, color);
			result = true;
		}

		nk_combo_end(m_spContext.get());
	}

	return result;
}

void NuklearUI::SetDefaultLayout()
{
	nk_layout_row_dynamic(m_spContext.get(), kDefaultRowHeight, 1);
}
