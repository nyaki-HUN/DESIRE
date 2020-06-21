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
	for(int i = 0; i < NK_KEY_MAX; i++)
	{
		ctx->input.keyboard.keys[i].down = 0;
	}

	for(const Keyboard& keyboard : Modules::Input->GetKeyboards())
	{
		const bool keyCtrl = keyboard.IsDown(KEY_LCONTROL) || keyboard.IsDown(KEY_RCONTROL);

		// Key down
		ctx->input.keyboard.keys[NK_KEY_SHIFT].down += keyboard.IsDown(KEY_LSHIFT) || keyboard.IsDown(KEY_RSHIFT);
		ctx->input.keyboard.keys[NK_KEY_SHIFT].clicked += keyboard.GetPressedCount(KEY_LSHIFT) + keyboard.GetPressedCount(KEY_RSHIFT);

		ctx->input.keyboard.keys[NK_KEY_CTRL].down += keyCtrl;
		ctx->input.keyboard.keys[NK_KEY_CTRL].clicked += keyboard.GetPressedCount(KEY_LCONTROL) + keyboard.GetPressedCount(KEY_RCONTROL);

		ctx->input.keyboard.keys[NK_KEY_DEL].down += keyboard.IsDown(KEY_DELETE);
		ctx->input.keyboard.keys[NK_KEY_DEL].clicked += keyboard.GetPressedCount(KEY_DELETE);

		ctx->input.keyboard.keys[NK_KEY_ENTER].down += keyboard.IsDown(KEY_RETURN) || keyboard.IsDown(KEY_NUMPADENTER);
		ctx->input.keyboard.keys[NK_KEY_ENTER].clicked += keyboard.GetPressedCount(KEY_RETURN) + keyboard.GetPressedCount(KEY_NUMPADENTER);

		ctx->input.keyboard.keys[NK_KEY_TAB].down += keyboard.IsDown(KEY_TAB);
		ctx->input.keyboard.keys[NK_KEY_TAB].clicked += keyboard.GetPressedCount(KEY_TAB);

		ctx->input.keyboard.keys[NK_KEY_BACKSPACE].down += keyboard.IsDown(KEY_BACKSPACE);
		ctx->input.keyboard.keys[NK_KEY_BACKSPACE].clicked += keyboard.GetPressedCount(KEY_BACKSPACE);

//		ctx->input.keyboard.keys[NK_KEY_TEXT_INSERT_MODE].down +=
//		ctx->input.keyboard.keys[NK_KEY_TEXT_INSERT_MODE].clicked +=

//		ctx->input.keyboard.keys[NK_KEY_TEXT_REPLACE_MODE].down +=
//		ctx->input.keyboard.keys[NK_KEY_TEXT_REPLACE_MODE].clicked +=

//		ctx->input.keyboard.keys[NK_KEY_TEXT_RESET_MODE].down +=
//		ctx->input.keyboard.keys[NK_KEY_TEXT_RESET_MODE].clicked +=

		ctx->input.keyboard.keys[NK_KEY_TEXT_LINE_START].down += keyboard.IsDown(KEY_HOME);
		ctx->input.keyboard.keys[NK_KEY_TEXT_LINE_START].clicked += keyboard.GetPressedCount(KEY_HOME);

		ctx->input.keyboard.keys[NK_KEY_TEXT_LINE_END].down += keyboard.IsDown(KEY_END);
		ctx->input.keyboard.keys[NK_KEY_TEXT_LINE_END].clicked += keyboard.GetPressedCount(KEY_END);

//		ctx->input.keyboard.keys[NK_KEY_TEXT_START].down +=
//		ctx->input.keyboard.keys[NK_KEY_TEXT_START].clicked +=

//		ctx->input.keyboard.keys[NK_KEY_TEXT_END].down +=
//		ctx->input.keyboard.keys[NK_KEY_TEXT_END].clicked +=

		ctx->input.keyboard.keys[NK_KEY_SCROLL_START].down += keyboard.IsDown(KEY_HOME);
		ctx->input.keyboard.keys[NK_KEY_SCROLL_START].clicked += keyboard.GetPressedCount(KEY_HOME);

		ctx->input.keyboard.keys[NK_KEY_SCROLL_END].down += keyboard.IsDown(KEY_END);
		ctx->input.keyboard.keys[NK_KEY_SCROLL_END].clicked += keyboard.GetPressedCount(KEY_END);

		ctx->input.keyboard.keys[NK_KEY_SCROLL_DOWN].down += keyboard.IsDown(KEY_PGDOWN);
		ctx->input.keyboard.keys[NK_KEY_SCROLL_DOWN].clicked += keyboard.GetPressedCount(KEY_PGDOWN);

		ctx->input.keyboard.keys[NK_KEY_SCROLL_UP].down += keyboard.IsDown(KEY_PGUP);
		ctx->input.keyboard.keys[NK_KEY_SCROLL_UP].clicked += keyboard.GetPressedCount(KEY_PGUP);

		if(keyCtrl)
		{
			ctx->input.keyboard.keys[NK_KEY_COPY].down += keyboard.IsDown(KEY_C);
			ctx->input.keyboard.keys[NK_KEY_COPY].clicked += keyboard.GetPressedCount(KEY_C);

			ctx->input.keyboard.keys[NK_KEY_CUT].down += keyboard.IsDown(KEY_X);
			ctx->input.keyboard.keys[NK_KEY_CUT].clicked += keyboard.GetPressedCount(KEY_X);

			ctx->input.keyboard.keys[NK_KEY_PASTE].down += keyboard.IsDown(KEY_V);
			ctx->input.keyboard.keys[NK_KEY_PASTE].clicked += keyboard.GetPressedCount(KEY_V);

			ctx->input.keyboard.keys[NK_KEY_TEXT_UNDO].down += keyboard.IsDown(KEY_Z);
			ctx->input.keyboard.keys[NK_KEY_TEXT_UNDO].clicked += keyboard.GetPressedCount(KEY_Z);

			ctx->input.keyboard.keys[NK_KEY_TEXT_REDO].down += keyboard.IsDown(KEY_Y);
			ctx->input.keyboard.keys[NK_KEY_TEXT_REDO].clicked += keyboard.GetPressedCount(KEY_Y);

			ctx->input.keyboard.keys[NK_KEY_TEXT_SELECT_ALL].down += keyboard.IsDown(KEY_A);
			ctx->input.keyboard.keys[NK_KEY_TEXT_SELECT_ALL].clicked += keyboard.GetPressedCount(KEY_A);

			ctx->input.keyboard.keys[NK_KEY_TEXT_WORD_LEFT].down += keyboard.IsDown(KEY_LEFT);
			ctx->input.keyboard.keys[NK_KEY_TEXT_WORD_LEFT].clicked += keyboard.GetPressedCount(KEY_LEFT);

			ctx->input.keyboard.keys[NK_KEY_TEXT_WORD_RIGHT].down += keyboard.IsDown(KEY_RIGHT);
			ctx->input.keyboard.keys[NK_KEY_TEXT_WORD_RIGHT].clicked += keyboard.GetPressedCount(KEY_RIGHT);
		}
		else
		{
			ctx->input.keyboard.keys[NK_KEY_UP].down += keyboard.IsDown(KEY_UP);
			ctx->input.keyboard.keys[NK_KEY_UP].clicked += keyboard.GetPressedCount(KEY_UP);

			ctx->input.keyboard.keys[NK_KEY_DOWN].down += keyboard.IsDown(KEY_DOWN);
			ctx->input.keyboard.keys[NK_KEY_DOWN].clicked += keyboard.GetPressedCount(KEY_DOWN);

			ctx->input.keyboard.keys[NK_KEY_LEFT].down += keyboard.IsDown(KEY_LEFT);
			ctx->input.keyboard.keys[NK_KEY_LEFT].clicked += keyboard.GetPressedCount(KEY_LEFT);

			ctx->input.keyboard.keys[NK_KEY_RIGHT].down += keyboard.IsDown(KEY_RIGHT);
			ctx->input.keyboard.keys[NK_KEY_RIGHT].clicked += keyboard.GetPressedCount(KEY_RIGHT);
		}
	}

	const String& typedCharacters = Modules::Input->GetTypingCharacters();
	for(size_t i = 0; typedCharacters.Length(); ++i)
	{
		nk_input_unicode(ctx.get(), typedCharacters.Str()[i]);
	}

	// Mouse
	const Vector2& mousePos = Modules::Input->GetOsMouseCursorPos();
	const int x = static_cast<int>(mousePos.GetX());
	const int y = static_cast<int>(mousePos.GetY());
	for(const Mouse& mouse : Modules::Input->GetMouses())
	{
		nk_input_button(ctx.get(), NK_BUTTON_LEFT, x, y, mouse.IsDown(Mouse::Button_Left));
		nk_input_button(ctx.get(), NK_BUTTON_MIDDLE, x, y, mouse.IsDown(Mouse::Button_Middle));
		nk_input_button(ctx.get(), NK_BUTTON_RIGHT, x, y, mouse.IsDown(Mouse::Button_Right));
	}

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

	{
		nk_buffer ibuf;
		nk_buffer vbuf;
		nk_buffer_init_fixed(&ibuf, mesh->indices.get(), mesh->maxNumOfIndices);
		nk_buffer_init_fixed(&vbuf, mesh->vertices.get(), mesh->maxNumOfVertices);
		nk_convert(ctx.get(), cmdBuffer.get(), &vbuf, &ibuf, convertConfig.get());
	}

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

		mesh->numIndices = pCmd->elem_count;
		material->ChangeTexture(0, *static_cast<const std::shared_ptr<Texture>*>(pCmd->texture.ptr));

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
}

bool NuklearUI::Button(const String& label, const Vector2& size)
{
	return nk_button_label(ctx.get(), label.Str());
}

bool NuklearUI::RadioButton(const String& label, bool isActive)
{
	return false;
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
	return false;
}

bool NuklearUI::Slider(const String& label, float& value, float minValue, float maxValue)
{
	return false;
}

bool NuklearUI::Checkbox(const String& label, bool& isChecked)
{
	return false;
}

void NuklearUI::SameLine()
{
}
