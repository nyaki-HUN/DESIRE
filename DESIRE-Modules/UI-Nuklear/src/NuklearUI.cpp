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
	mesh = std::make_unique<DynamicMesh>();
	mesh->vertexLayout.Reserve(3);
	mesh->vertexLayout.EmplaceAdd(Mesh::EAttrib::Position, 2, Mesh::EAttribType::Float);
	mesh->vertexLayout.EmplaceAdd(Mesh::EAttrib::Texcoord0, 2, Mesh::EAttribType::Float);
	mesh->vertexLayout.EmplaceAdd(Mesh::EAttrib::Color, 4, Mesh::EAttribType::Uint8);
	mesh->CalculateStrideFromVertexLayout();
	mesh->maxNumOfIndices = 128 * 1024;
	mesh->maxNumOfVertices = 256 * 1024;
	mesh->indices = std::make_unique<uint16_t[]>(mesh->maxNumOfIndices);
	mesh->vertices = std::make_unique<float[]>(mesh->GetTotalBytesOfVertexData() / sizeof(float));
	memset(mesh->indices.get(), 0, mesh->GetTotalBytesOfIndexData());
	memset(mesh->vertices.get(), 0, mesh->GetTotalBytesOfVertexData());

	static_assert(sizeof(nk_draw_index) == sizeof(uint16_t), "Conversion is required for index buffer");

	static const nk_draw_vertex_layout_element s_nkVertexLayout[] =
	{
		{ NK_VERTEX_POSITION, NK_FORMAT_FLOAT, 0 },
		{ NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, 2 * sizeof(float) },
		{ NK_VERTEX_COLOR, NK_FORMAT_RGBA32, 4 * sizeof(float) },
		{ NK_VERTEX_LAYOUT_END }
	};

	convertConfig = std::make_unique<nk_convert_config>();
	convertConfig->vertex_layout = s_nkVertexLayout;
	convertConfig->vertex_size = mesh->stride;
	convertConfig->vertex_alignment = MemorySystem::kDefaultAlignment;
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
	fontTexture = std::make_shared<Texture>(static_cast<uint16_t>(width), static_cast<uint16_t>(height), Texture::EFormat::RGBA8);
	fontTexture->data = MemoryBuffer::CreateFromDataCopy(pTextureData, width * height * 4u);
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

void NuklearUI::BeginFrame(OSWindow* pWindow)
{
	ctx->delta_time_seconds = Modules::Application->GetTimer()->GetSecDelta();

	nk_input_begin(ctx.get());

	// Keyboard
	nk_input_key(ctx.get(), NK_KEY_SHIFT, Modules::Input->IsKeyDown(KEY_LSHIFT) || Modules::Input->IsKeyDown(KEY_RSHIFT));
	nk_input_key(ctx.get(), NK_KEY_CTRL, Modules::Input->IsKeyDown(KEY_LCONTROL) || Modules::Input->IsKeyDown(KEY_RCONTROL));
	nk_input_key(ctx.get(), NK_KEY_DEL, Modules::Input->IsKeyDown(KEY_DELETE));
	nk_input_key(ctx.get(), NK_KEY_ENTER, Modules::Input->IsKeyDown(KEY_RETURN) || Modules::Input->IsKeyDown(KEY_NUMPADENTER));
	nk_input_key(ctx.get(), NK_KEY_TAB, Modules::Input->IsKeyDown(KEY_TAB));
	nk_input_key(ctx.get(), NK_KEY_BACKSPACE, Modules::Input->IsKeyDown(KEY_BACKSPACE));
	nk_input_key(ctx.get(), NK_KEY_COPY, Modules::Input->IsKeyDown(KEY_C, EKeyModifier::Ctrl));
	nk_input_key(ctx.get(), NK_KEY_CUT, Modules::Input->IsKeyDown(KEY_X, EKeyModifier::Ctrl));
	nk_input_key(ctx.get(), NK_KEY_PASTE, Modules::Input->IsKeyDown(KEY_V, EKeyModifier::Ctrl));
	nk_input_key(ctx.get(), NK_KEY_UP, Modules::Input->IsKeyDown(KEY_UP, EKeyModifier::None));
	nk_input_key(ctx.get(), NK_KEY_DOWN, Modules::Input->IsKeyDown(KEY_DOWN, EKeyModifier::None));
	nk_input_key(ctx.get(), NK_KEY_LEFT, Modules::Input->IsKeyDown(KEY_LEFT, EKeyModifier::None));
	nk_input_key(ctx.get(), NK_KEY_RIGHT, Modules::Input->IsKeyDown(KEY_RIGHT, EKeyModifier::None));
//	nk_input_key(ctx.get(), NK_KEY_TEXT_INSERT_MODE, Modules::Input->IsKeyDown());
//	nk_input_key(ctx.get(), NK_KEY_TEXT_REPLACE_MODE, Modules::Input->IsKeyDown());
//	nk_input_key(ctx.get(), NK_KEY_TEXT_RESET_MODE, Modules::Input->IsKeyDown());
	nk_input_key(ctx.get(), NK_KEY_TEXT_LINE_START, Modules::Input->IsKeyDown(KEY_HOME));
	nk_input_key(ctx.get(), NK_KEY_TEXT_LINE_END, Modules::Input->IsKeyDown(KEY_END));
//	nk_input_key(ctx.get(), NK_KEY_TEXT_START, Modules::Input->IsKeyDown());
//	nk_input_key(ctx.get(), NK_KEY_TEXT_END, Modules::Input->IsKeyDown());
	nk_input_key(ctx.get(), NK_KEY_TEXT_UNDO, Modules::Input->IsKeyDown(KEY_Z, EKeyModifier::Ctrl));
	nk_input_key(ctx.get(), NK_KEY_TEXT_REDO, Modules::Input->IsKeyDown(KEY_Y, EKeyModifier::Ctrl));
	nk_input_key(ctx.get(), NK_KEY_TEXT_SELECT_ALL, Modules::Input->IsKeyDown(KEY_A, EKeyModifier::Ctrl));
	nk_input_key(ctx.get(), NK_KEY_TEXT_WORD_LEFT, Modules::Input->IsKeyDown(KEY_LEFT, EKeyModifier::Ctrl));
	nk_input_key(ctx.get(), NK_KEY_TEXT_WORD_RIGHT, Modules::Input->IsKeyDown(KEY_RIGHT, EKeyModifier::Ctrl));
	nk_input_key(ctx.get(), NK_KEY_SCROLL_START, Modules::Input->IsKeyDown(KEY_HOME));
	nk_input_key(ctx.get(), NK_KEY_SCROLL_END, Modules::Input->IsKeyDown(KEY_END));
	nk_input_key(ctx.get(), NK_KEY_SCROLL_DOWN, Modules::Input->IsKeyDown(KEY_PGDOWN));
	nk_input_key(ctx.get(), NK_KEY_SCROLL_UP, Modules::Input->IsKeyDown(KEY_PGUP));

	const String& typedCharacters = Modules::Input->GetTypingCharacters();
	for(size_t i = 0; i < typedCharacters.Length(); ++i)
	{
		nk_input_unicode(ctx.get(), typedCharacters.Str()[i]);
	}

	// Mouse
	const Vector2& mousePos = Modules::Input->GetOsMouseCursorPos();
	const int x = static_cast<int>(mousePos.GetX());
	const int y = static_cast<int>(mousePos.GetY());
	nk_input_button(ctx.get(), NK_BUTTON_LEFT, x, y, Modules::Input->IsMouseButtonDown(Mouse::Button_Left));
	nk_input_button(ctx.get(), NK_BUTTON_MIDDLE, x, y, Modules::Input->IsMouseButtonDown(Mouse::Button_Middle));
	nk_input_button(ctx.get(), NK_BUTTON_RIGHT, x, y, Modules::Input->IsMouseButtonDown(Mouse::Button_Right));

	nk_input_end(ctx.get());
}

void NuklearUI::EndFrame()
{
}

void NuklearUI::Render()
{
	Modules::Render->SetDepthWriteEnabled(false);
	Modules::Render->SetCullMode(Render::ECullMode::None);
	Modules::Render->SetBlendMode(Render::EBlend::SrcAlpha, Render::EBlend::InvSrcAlpha, Render::EBlendOp::Add);

	// Update mesh with packed buffers for contiguous indices and vertices
	mesh->numIndices = 0;
	mesh->numVertices = 0;
	nk_buffer indexBuffer;
	nk_buffer vertexBuffer;
	nk_buffer_init_fixed(&indexBuffer, mesh->indices.get(), mesh->maxNumOfIndices);
	nk_buffer_init_fixed(&vertexBuffer, mesh->vertices.get(), mesh->maxNumOfVertices);
	nk_convert(ctx.get(), cmdBuffer.get(), &vertexBuffer, &indexBuffer, convertConfig.get());

	mesh->isIndexDataUpdateRequired = true;
	mesh->isVertexDataUpdateRequired = true;
	Modules::Render->UpdateDynamicMesh(mesh.get());

	mesh->indexOffset = 0;
	mesh->vertexOffset = 0;

	const nk_draw_command* pCmd = nullptr;
	nk_draw_foreach(pCmd, ctx.get(), cmdBuffer.get())
	{
		if(pCmd->elem_count == 0)
		{
			continue;
		}

		const uint16_t clipX = static_cast<uint16_t>(std::max(0.0f, pCmd->clip_rect.x));
		const uint16_t clipY = static_cast<uint16_t>(std::max(0.0f, pCmd->clip_rect.y));
		Modules::Render->SetScissor(clipX, clipY, static_cast<uint16_t>(pCmd->clip_rect.w), static_cast<uint16_t>(pCmd->clip_rect.h));

		material->ChangeTexture(0, *static_cast<const std::shared_ptr<Texture>*>(pCmd->texture.ptr));

		mesh->numIndices = pCmd->elem_count;
		Modules::Render->RenderMesh(mesh.get(), material.get());

		mesh->indexOffset += mesh->numIndices;
	}

	Modules::Render->SetScissor();

	nk_clear(ctx.get());
	nk_buffer_clear(cmdBuffer.get());
}

void NuklearUI::BeginWindow(const String& label)
{
	nk_flags flags = NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE;
	nk_begin(ctx.get(), label.Str(), nk_rect(50, 50, 800, 600), flags);
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

void NuklearUI::SameLine()
{
}
