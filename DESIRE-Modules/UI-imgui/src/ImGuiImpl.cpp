#include "ImGuiImpl.h"

#include "Engine/Core/assert.h"
#include "Engine/Core/IWindow.h"
#include "Engine/Core/Timer.h"
#include "Engine/Input/Input.h"
#include "Engine/Render/Material.h"
#include "Engine/Render/Render.h"
#include "Engine/Resource/ResourceManager.h"
#include "Engine/Resource/Mesh.h"
#include "Engine/Resource/Shader.h"
#include "Engine/Resource/Texture.h"

#include "imgui.h"

#include <algorithm>	// for std::max()

ImGuiImpl::ImGuiImpl()
{

}

ImGuiImpl::~ImGuiImpl()
{

}

void ImGuiImpl::Init()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ASSERT(io.UserData == nullptr && "ImGui is already initialized");

	io.KeyMap[ImGuiKey_Tab] = KEY_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = KEY_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = KEY_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = KEY_UP;
	io.KeyMap[ImGuiKey_DownArrow] = KEY_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = KEY_PGUP;
	io.KeyMap[ImGuiKey_PageDown] = KEY_PGDOWN;
	io.KeyMap[ImGuiKey_Home] = KEY_HOME;
	io.KeyMap[ImGuiKey_End] = KEY_END;
	io.KeyMap[ImGuiKey_Insert] = KEY_INSERT;
	io.KeyMap[ImGuiKey_Delete] = KEY_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = KEY_BACKSPACE;
	io.KeyMap[ImGuiKey_Space] = KEY_SPACE;
	io.KeyMap[ImGuiKey_Enter] = KEY_RETURN;
	io.KeyMap[ImGuiKey_Escape] = KEY_ESCAPE;
	io.KeyMap[ImGuiKey_A] = KEY_A;
	io.KeyMap[ImGuiKey_C] = KEY_C;
	io.KeyMap[ImGuiKey_V] = KEY_V;
	io.KeyMap[ImGuiKey_X] = KEY_X;
	io.KeyMap[ImGuiKey_Y] = KEY_Y;
	io.KeyMap[ImGuiKey_Z] = KEY_Z;

	io.IniFilename = nullptr;
	io.RenderDrawListsFn = ImGuiImpl::RenderDrawListsCallback;

	// Dynamic mesh for the draw list
	mesh = std::make_unique<DynamicMesh>();
	mesh->vertexDecl.reserve(3);
	mesh->vertexDecl.emplace_back(Mesh::EAttrib::POSITION, 2, Mesh::EAttribType::FLOAT);
	mesh->vertexDecl.emplace_back(Mesh::EAttrib::TEXCOORD0, 2, Mesh::EAttribType::FLOAT);
	mesh->vertexDecl.emplace_back(Mesh::EAttrib::COLOR, 4, Mesh::EAttribType::UINT8);
	mesh->CalculateStrideFromVertexDecl();
	mesh->maxNumOfIndices = 30000;
	mesh->maxNumOfVertices = 20000;

	mesh->indices = std::make_unique<uint16_t[]>(mesh->GetMaxSizeOfIndices());
	mesh->vertices = std::make_unique<float[]>(mesh->GetMaxSizeOfVertices());
	memset(mesh->indices.get(), 0, mesh->GetMaxSizeOfIndices());
	memset(mesh->vertices.get(), 0, mesh->GetMaxSizeOfVertices());

	ASSERT(sizeof(ImDrawIdx) == sizeof(uint16_t) && "Conversion is required for index buffer");
	ASSERT(sizeof(ImDrawVert) == mesh->stride && "ImDrawVert struct layout has changed");

	// Setup material
	material = std::make_unique<Material>();
	material->vertexShader = ResourceManager::Get()->GetShader("vs_ocornut_imgui");
	material->fragmentShader = ResourceManager::Get()->GetShader("fs_ocornut_imgui");

	// Setup font texture
	unsigned char *textureData;
	int width, height;
	io.Fonts->AddFontDefault();
	io.Fonts->GetTexDataAsRGBA32(&textureData, &width, &height);

	fontTexture = std::make_shared<Texture>((uint16_t)width, (uint16_t)height, Texture::EFormat::RGBA8);
	fontTexture->data = MemoryBuffer::CreateFromDataCopy(textureData, width * height * 4u);
	material->AddTexture(fontTexture);
	io.Fonts->TexID = &fontTexture;
	
	// Cleanup (don't clear the input data if you want to append new fonts later)
	io.Fonts->ClearInputData();
	io.Fonts->ClearTexData();
}

void ImGuiImpl::Kill()
{
	ImGui::DestroyContext();

	mesh = nullptr;
	material = nullptr;
	fontTexture = nullptr;
}

void ImGuiImpl::NewFrame(IWindow *window)
{
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(window->GetWidth(), window->GetHeight());
	io.DeltaTime = Timer::Get()->GetSecDelta();
	io.UserData = window;
	io.ImeWindowHandle = window->GetHandle();

	// Keyboard
	const std::vector<Keyboard>& keyboards = Input::Get()->GetKeyboards();
	if(!keyboards.empty())
	{
		const Keyboard& keyboard = keyboards.back();
		io.KeyCtrl = keyboard.IsDown(KEY_LCONTROL) || keyboard.IsDown(KEY_RCONTROL);
		io.KeyShift = keyboard.IsDown(KEY_LSHIFT) || keyboard.IsDown(KEY_RSHIFT);
		io.KeyAlt = keyboard.IsDown(KEY_LALT) || keyboard.IsDown(KEY_RALT);
		io.KeySuper = keyboard.IsDown(KEY_LWIN) || keyboard.IsDown(KEY_RWIN);

		for(int keyCode : io.KeyMap)
		{
			io.KeysDown[keyCode] = keyboard.IsDown(keyCode);
		}
	}

	const char *typedCharacters = Input::Get()->GetTypingCharacters();
	for(int i = 0; typedCharacters[i] != '\0'; ++i)
	{
		io.AddInputCharacter((ImWchar)typedCharacters[i]);
	}

	// Mouse
	const Vector2& mousePos = Input::Get()->GetOsMouseCursorPos();
	io.MousePos = ImVec2(mousePos.GetX(), mousePos.GetY());

	io.MouseWheel = 0.0f;
	for(int i = 0; i < (int)DESIRE_ASIZEOF(io.MouseDown); ++i)
	{
		io.MouseDown[i] = false;
	}
	for(const Mouse& mouse : Input::Get()->GetMouses())
	{
		io.MouseWheel += mouse.GetAxisDelta(Mouse::WHEEL);
		for(int i = 0; i < (int)DESIRE_ASIZEOF(io.MouseDown); ++i)
		{
			io.MouseDown[i] |= mouse.IsDown(i);
		}
	}

	ImGui::NewFrame();
}

void ImGuiImpl::EndFrame()
{
	ImGuiIO& io = ImGui::GetIO();
	IWindow *window = static_cast<IWindow*>(io.UserData);
	switch(ImGui::GetMouseCursor())
	{
		case ImGuiMouseCursor_Arrow:		window->SetCursor(IWindow::CURSOR_ARROW); break;
		case ImGuiMouseCursor_TextInput:	window->SetCursor(IWindow::CURSOR_IBEAM); break;
		case ImGuiMouseCursor_Move:			window->SetCursor(IWindow::CURSOR_MOVE); break;
		case ImGuiMouseCursor_ResizeNS:		window->SetCursor(IWindow::CURSOR_SIZE_NS); break;
		case ImGuiMouseCursor_ResizeEW:		window->SetCursor(IWindow::CURSOR_SIZE_WE); break;
		case ImGuiMouseCursor_ResizeNESW:	window->SetCursor(IWindow::CURSOR_SIZE_BOTTOMLEFT); break;
		case ImGuiMouseCursor_ResizeNWSE:	window->SetCursor(IWindow::CURSOR_SIZE_BOTTOMRIGHT); break;
	};

	ImGui::Render();
}

void ImGuiImpl::DoRender(ImDrawData *drawData)
{
	Render *render = Render::Get();

	render->SetDepthWriteEnabled(false);
	render->SetCullMode(Render::ECullMode::NONE);
	render->SetBlendMode(Render::EBlend::SRC_ALPHA, Render::EBlend::INV_SRC_ALPHA, Render::EBlendOp::ADD);

	// Update mesh with packed buffers for contiguous indices and vertices
	ASSERT((uint32_t)drawData->TotalIdxCount <= mesh->maxNumOfIndices);
	ASSERT((uint32_t)drawData->TotalVtxCount <= mesh->maxNumOfVertices);
	mesh->numIndices = 0;
	mesh->numVertices = 0;
	for(int cmdIdx = 0; cmdIdx < drawData->CmdListsCount; ++cmdIdx)
	{
		const ImDrawList *drawList = drawData->CmdLists[cmdIdx];
		if(mesh->numIndices + drawList->IdxBuffer.size() > mesh->maxNumOfIndices ||
			mesh->numVertices + drawList->VtxBuffer.size() > mesh->maxNumOfVertices)
		{
			break;
		}

		memcpy(&mesh->indices[mesh->numIndices], drawList->IdxBuffer.Data, drawList->IdxBuffer.size() * sizeof(uint16_t));
		mesh->numIndices += (uint32_t)drawList->IdxBuffer.size();

		memcpy((uint8_t*)mesh->vertices.get() + mesh->numVertices * mesh->stride, drawList->VtxBuffer.Data, drawList->VtxBuffer.size() * mesh->stride);
		mesh->numVertices += (uint32_t)drawList->VtxBuffer.size();
	}
	mesh->isIndexDataUpdateRequired = true;
	mesh->isVertexDataUpdateRequired = true;
	render->UpdateDynamicMesh(mesh.get());

	mesh->indexOffset = 0;
	mesh->vertexOffset = 0;
	for(int cmdIdx = 0; cmdIdx < drawData->CmdListsCount; ++cmdIdx)
	{
		const ImDrawList *drawList = drawData->CmdLists[cmdIdx];
		mesh->numVertices = (uint32_t)drawList->VtxBuffer.size();

		for(const ImDrawCmd& cmd : drawList->CmdBuffer)
		{
			const uint16_t clipX = std::max<uint16_t>(0, (uint16_t)cmd.ClipRect.x);
			const uint16_t clipY = std::max<uint16_t>(0, (uint16_t)cmd.ClipRect.y);
			render->SetScissor(clipX, clipY, (uint16_t)(cmd.ClipRect.z - clipX), (uint16_t)(cmd.ClipRect.w - clipY));

			if(cmd.UserCallback)
			{
				cmd.UserCallback(drawList, &cmd);
				continue;
			}

			mesh->numIndices = cmd.ElemCount;
			material->ChangeTexture(0, *static_cast<const std::shared_ptr<Texture>*>(cmd.TextureId));

			render->RenderMesh(mesh.get(), material.get());

			mesh->indexOffset += mesh->numIndices;
		}

		mesh->vertexOffset += mesh->numVertices;
	}

	render->SetScissor();
}

void ImGuiImpl::RenderDrawListsCallback(ImDrawData *drawData)
{
	ImGuiImpl::Get()->DoRender(drawData);
}
