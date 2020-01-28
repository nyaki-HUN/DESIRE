#include "stdafx_imgui.h"
#include "ImGuiImpl.h"

#include "Engine/Application/Application.h"
#include "Engine/Application/OSWindow.h"

#include "Engine/Core/Memory/MemorySystem.h"
#include "Engine/Core/Timer.h"

#include "Engine/Input/Input.h"

#include "Engine/Render/Material.h"
#include "Engine/Render/Render.h"
#include "Engine/Resource/ResourceManager.h"
#include "Engine/Resource/Mesh.h"
#include "Engine/Resource/Shader.h"
#include "Engine/Resource/Texture.h"

ImGuiImpl::ImGuiImpl()
{
	ImGui::SetAllocatorFunctions(&ImGuiImpl::MallocWrapper, &ImGuiImpl::FreeWrapper, this);
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
	io.KeyMap[ImGuiKey_KeyPadEnter] = KEY_NUMPADENTER;
	io.KeyMap[ImGuiKey_A] = KEY_A;
	io.KeyMap[ImGuiKey_C] = KEY_C;
	io.KeyMap[ImGuiKey_V] = KEY_V;
	io.KeyMap[ImGuiKey_X] = KEY_X;
	io.KeyMap[ImGuiKey_Y] = KEY_Y;
	io.KeyMap[ImGuiKey_Z] = KEY_Z;

	io.IniFilename = nullptr;
	io.ConfigWindowsMoveFromTitleBarOnly = true;

	// Dynamic mesh for the draw list
	mesh = std::make_unique<DynamicMesh>();
	mesh->vertexDecl.Reserve(3);
	mesh->vertexDecl.EmplaceAdd(Mesh::EAttrib::Position, 2, Mesh::EAttribType::Float);
	mesh->vertexDecl.EmplaceAdd(Mesh::EAttrib::Texcoord0, 2, Mesh::EAttribType::Float);
	mesh->vertexDecl.EmplaceAdd(Mesh::EAttrib::Color, 4, Mesh::EAttribType::Uint8);
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
	material->vertexShader = Modules::ResourceManager->GetShader("vs_ocornut_imgui");
	material->fragmentShader = Modules::ResourceManager->GetShader("fs_ocornut_imgui");

	// Setup font texture
	unsigned char* textureData = nullptr;
	int width, height;
	io.Fonts->AddFontDefault();
	io.Fonts->GetTexDataAsRGBA32(&textureData, &width, &height);

	fontTexture = std::make_shared<Texture>(static_cast<uint16_t>(width), static_cast<uint16_t>(height), Texture::EFormat::RGBA8);
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

void ImGuiImpl::NewFrame(OSWindow* window)
{
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(window->GetWidth(), window->GetHeight());
	io.DeltaTime = std::max(FLT_MIN, Modules::Application->GetTimer()->GetSecDelta());		// imgui needs a positive delta time
	io.UserData = window;
	io.ImeWindowHandle = window->GetHandle();

	// Keyboard
	io.KeyCtrl = false;
	io.KeyShift = false;
	io.KeyAlt = false;
	io.KeySuper = false;
	memset(io.KeysDown, 0, sizeof(io.KeysDown));
	for(const Keyboard& keyboard : Modules::Input->GetKeyboards())
	{
		io.KeyCtrl |= keyboard.IsDown(KEY_LCONTROL) || keyboard.IsDown(KEY_RCONTROL);
		io.KeyShift |= keyboard.IsDown(KEY_LSHIFT) || keyboard.IsDown(KEY_RSHIFT);
		io.KeyAlt |= keyboard.IsDown(KEY_LALT) || keyboard.IsDown(KEY_RALT);
		io.KeySuper |= keyboard.IsDown(KEY_LWIN) || keyboard.IsDown(KEY_RWIN);

		for(int keyCode : io.KeyMap)
		{
			io.KeysDown[keyCode] |= keyboard.IsDown(keyCode);
		}
	}

	const char* typedCharacters = Modules::Input->GetTypingCharacters();
	for(int i = 0; typedCharacters[i] != '\0'; ++i)
	{
		io.AddInputCharacter(typedCharacters[i]);
	}

	// Mouse
	const Vector2& mousePos = Modules::Input->GetOsMouseCursorPos();
	io.MousePos = ImVec2(mousePos.GetX(), mousePos.GetY());

	memset(io.MouseDown, 0, sizeof(io.MouseDown));
	io.MouseWheel = 0.0f;
	io.MouseWheelH = 0.0f;
	for(const Mouse& mouse : Modules::Input->GetMouses())
	{
		io.MouseWheel += mouse.GetAxisDelta(Mouse::Wheel);

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
	OSWindow* window = static_cast<OSWindow*>(io.UserData);
	switch(ImGui::GetMouseCursor())
	{
		case ImGuiMouseCursor_Arrow:		window->SetCursor(OSWindow::CURSOR_ARROW); break;
		case ImGuiMouseCursor_TextInput:	window->SetCursor(OSWindow::CURSOR_IBEAM); break;
		case ImGuiMouseCursor_ResizeAll:	window->SetCursor(OSWindow::CURSOR_MOVE); break;
		case ImGuiMouseCursor_ResizeNS:		window->SetCursor(OSWindow::CURSOR_SIZE_NS); break;
		case ImGuiMouseCursor_ResizeEW:		window->SetCursor(OSWindow::CURSOR_SIZE_WE); break;
		case ImGuiMouseCursor_ResizeNESW:	window->SetCursor(OSWindow::CURSOR_SIZE_BOTTOMLEFT); break;
		case ImGuiMouseCursor_ResizeNWSE:	window->SetCursor(OSWindow::CURSOR_SIZE_BOTTOMRIGHT); break;
		case ImGuiMouseCursor_Hand:			window->SetCursor(OSWindow::CURSOR_HAND); break;
		case ImGuiMouseCursor_NotAllowed:	window->SetCursor(OSWindow::CURSOR_NOT_ALLOWED); break;
	};

	ImGui::Render();
	ImDrawData* drawData = ImGui::GetDrawData();
	if(drawData != nullptr)
	{
		DoRender(drawData);
	}
}

void ImGuiImpl::DoRender(ImDrawData* drawData)
{
	Modules::Render->SetDepthWriteEnabled(false);
	Modules::Render->SetCullMode(Render::ECullMode::None);
	Modules::Render->SetBlendMode(Render::EBlend::SrcAlpha, Render::EBlend::InvSrcAlpha, Render::EBlendOp::Add);

	// Update mesh with packed buffers for contiguous indices and vertices
	ASSERT(static_cast<uint32_t>(drawData->TotalIdxCount) <= mesh->maxNumOfIndices);
	ASSERT(static_cast<uint32_t>(drawData->TotalVtxCount) <= mesh->maxNumOfVertices);
	mesh->numIndices = 0;
	mesh->numVertices = 0;
	for(int cmdIdx = 0; cmdIdx < drawData->CmdListsCount; ++cmdIdx)
	{
		const ImDrawList* drawList = drawData->CmdLists[cmdIdx];
		if(mesh->numIndices + drawList->IdxBuffer.size() > mesh->maxNumOfIndices ||
			mesh->numVertices + drawList->VtxBuffer.size() > mesh->maxNumOfVertices)
		{
			break;
		}

		memcpy(&mesh->indices[mesh->numIndices], drawList->IdxBuffer.Data, drawList->IdxBuffer.size() * sizeof(uint16_t));
		mesh->numIndices += static_cast<uint32_t>(drawList->IdxBuffer.size());

		memcpy(reinterpret_cast<uint8_t*>(mesh->vertices.get()) + mesh->numVertices * mesh->stride, drawList->VtxBuffer.Data, drawList->VtxBuffer.size() * mesh->stride);
		mesh->numVertices += static_cast<uint32_t>(drawList->VtxBuffer.size());
	}
	mesh->isIndexDataUpdateRequired = true;
	mesh->isVertexDataUpdateRequired = true;
	Modules::Render->UpdateDynamicMesh(mesh.get());

	mesh->indexOffset = 0;
	mesh->vertexOffset = 0;
	for(int cmdIdx = 0; cmdIdx < drawData->CmdListsCount; ++cmdIdx)
	{
		const ImDrawList* drawList = drawData->CmdLists[cmdIdx];
		mesh->numVertices = static_cast<uint32_t>(drawList->VtxBuffer.size());

		for(const ImDrawCmd& cmd : drawList->CmdBuffer)
		{
			const uint16_t clipX = std::max<uint16_t>(0, static_cast<uint16_t>(cmd.ClipRect.x));
			const uint16_t clipY = std::max<uint16_t>(0, static_cast<uint16_t>(cmd.ClipRect.y));
			Modules::Render->SetScissor(clipX, clipY, static_cast<uint16_t>(cmd.ClipRect.z - clipX), static_cast<uint16_t>(cmd.ClipRect.w - clipY));

			if(cmd.UserCallback)
			{
				cmd.UserCallback(drawList, &cmd);
				continue;
			}

			mesh->numIndices = cmd.ElemCount;
			material->ChangeTexture(0, *static_cast<const std::shared_ptr<Texture>*>(cmd.TextureId));

			Modules::Render->RenderMesh(mesh.get(), material.get());

			mesh->indexOffset += mesh->numIndices;
		}

		mesh->vertexOffset += mesh->numVertices;
	}

	Modules::Render->SetScissor();
}

void* ImGuiImpl::MallocWrapper(size_t size, void* userData)
{
	DESIRE_UNUSED(userData);
	return MemorySystem::Alloc(size);
}

void ImGuiImpl::FreeWrapper(void* ptr, void* userData)
{
	DESIRE_UNUSED(userData);
	MemorySystem::Free(ptr);
}
