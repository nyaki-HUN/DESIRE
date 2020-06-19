#include "stdafx_imgui.h"
#include "ImGuiUI.h"

#include "ImGuiCallbacks.h"

#include "Engine/Application/Application.h"
#include "Engine/Application/OSWindow.h"

#include "Engine/Core/FS/FileSystem.h"
#include "Engine/Core/FS/IReadFile.h"
#include "Engine/Core/Math/Vector3.h"
#include "Engine/Core/String/String.h"
#include "Engine/Core/Timer.h"

#include "Engine/Input/Input.h"

#include "Engine/Render/Material.h"
#include "Engine/Render/Render.h"

#include "Engine/Resource/ResourceManager.h"
#include "Engine/Resource/Mesh.h"
#include "Engine/Resource/Shader.h"
#include "Engine/Resource/Texture.h"

ImGuiUI::ImGuiUI()
{
}

ImGuiUI::~ImGuiUI()
{
}

void ImGuiUI::Init()
{
	ImGui::SetAllocatorFunctions(&ImGuiCallbacks::MallocWrapper, &ImGuiCallbacks::FreeWrapper, this);

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
	mesh->vertexLayout.Reserve(3);
	mesh->vertexLayout.EmplaceAdd(Mesh::EAttrib::Position, 2, Mesh::EAttribType::Float);
	mesh->vertexLayout.EmplaceAdd(Mesh::EAttrib::Texcoord0, 2, Mesh::EAttribType::Float);
	mesh->vertexLayout.EmplaceAdd(Mesh::EAttrib::Color, 4, Mesh::EAttribType::Uint8);
	mesh->CalculateStrideFromVertexLayout();
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

	// Setup fonts
	io.Fonts->AddFontDefault();

	// Setup font texture
	unsigned char* textureData = nullptr;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&textureData, &width, &height);

	fontTexture = std::make_shared<Texture>(static_cast<uint16_t>(width), static_cast<uint16_t>(height), Texture::EFormat::RGBA8);
	fontTexture->data = MemoryBuffer::CreateFromDataCopy(textureData, width * height * 4u);
	material->AddTexture(fontTexture);
	io.Fonts->TexID = &fontTexture;

	// Cleanup (don't clear the input data if you want to append new fonts later)
	io.Fonts->ClearInputData();
	io.Fonts->ClearTexData();
}

void ImGuiUI::Kill()
{
	ImGui::DestroyContext();

	mesh = nullptr;
	material = nullptr;
	fontTexture = nullptr;
}

void ImGuiUI::BeginFrame(OSWindow* pWindow)
{
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(pWindow->GetWidth(), pWindow->GetHeight());
	io.DeltaTime = std::max(FLT_MIN, Modules::Application->GetTimer()->GetSecDelta());		// imgui needs a positive delta time
	io.UserData = pWindow;
	io.ImeWindowHandle = pWindow->GetHandle();

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

void ImGuiUI::EndFrame()
{
	ImGuiIO& io = ImGui::GetIO();
	OSWindow* pWindow = static_cast<OSWindow*>(io.UserData);
	switch(ImGui::GetMouseCursor())
	{
		case ImGuiMouseCursor_Arrow:		pWindow->SetCursor(OSWindow::CURSOR_ARROW); break;
		case ImGuiMouseCursor_TextInput:	pWindow->SetCursor(OSWindow::CURSOR_IBEAM); break;
		case ImGuiMouseCursor_ResizeAll:	pWindow->SetCursor(OSWindow::CURSOR_MOVE); break;
		case ImGuiMouseCursor_ResizeNS:		pWindow->SetCursor(OSWindow::CURSOR_SIZE_NS); break;
		case ImGuiMouseCursor_ResizeEW:		pWindow->SetCursor(OSWindow::CURSOR_SIZE_WE); break;
		case ImGuiMouseCursor_ResizeNESW:	pWindow->SetCursor(OSWindow::CURSOR_SIZE_BOTTOMLEFT); break;
		case ImGuiMouseCursor_ResizeNWSE:	pWindow->SetCursor(OSWindow::CURSOR_SIZE_BOTTOMRIGHT); break;
		case ImGuiMouseCursor_Hand:			pWindow->SetCursor(OSWindow::CURSOR_HAND); break;
		case ImGuiMouseCursor_NotAllowed:	pWindow->SetCursor(OSWindow::CURSOR_NOT_ALLOWED); break;
	};

	ImGui::Render();
}

void ImGuiUI::Render()
{
	ImDrawData* pDrawData = ImGui::GetDrawData();
	if(pDrawData == nullptr)
	{
		return;
	}

	Modules::Render->SetDepthWriteEnabled(false);
	Modules::Render->SetCullMode(Render::ECullMode::None);
	Modules::Render->SetBlendMode(Render::EBlend::SrcAlpha, Render::EBlend::InvSrcAlpha, Render::EBlendOp::Add);

	// Update mesh with packed buffers for contiguous indices and vertices
	ASSERT(static_cast<uint32_t>(pDrawData->TotalIdxCount) <= mesh->maxNumOfIndices);
	ASSERT(static_cast<uint32_t>(pDrawData->TotalVtxCount) <= mesh->maxNumOfVertices);
	mesh->numIndices = 0;
	mesh->numVertices = 0;
	for(int cmdIdx = 0; cmdIdx < pDrawData->CmdListsCount; ++cmdIdx)
	{
		const ImDrawList* pDrawList = pDrawData->CmdLists[cmdIdx];
		if(mesh->numIndices + pDrawList->IdxBuffer.size() > mesh->maxNumOfIndices ||
			mesh->numVertices + pDrawList->VtxBuffer.size() > mesh->maxNumOfVertices)
		{
			break;
		}

		memcpy(&mesh->indices[mesh->numIndices], pDrawList->IdxBuffer.Data, pDrawList->IdxBuffer.size() * sizeof(uint16_t));
		mesh->numIndices += static_cast<uint32_t>(pDrawList->IdxBuffer.size());

		memcpy(reinterpret_cast<uint8_t*>(mesh->vertices.get()) + mesh->numVertices * mesh->stride, pDrawList->VtxBuffer.Data, pDrawList->VtxBuffer.size() * mesh->stride);
		mesh->numVertices += static_cast<uint32_t>(pDrawList->VtxBuffer.size());
	}
	mesh->isIndexDataUpdateRequired = true;
	mesh->isVertexDataUpdateRequired = true;
	Modules::Render->UpdateDynamicMesh(mesh.get());

	mesh->indexOffset = 0;
	mesh->vertexOffset = 0;
	for(int cmdIdx = 0; cmdIdx < pDrawData->CmdListsCount; ++cmdIdx)
	{
		const ImDrawList* pDrawList = pDrawData->CmdLists[cmdIdx];
		mesh->numVertices = static_cast<uint32_t>(pDrawList->VtxBuffer.size());

		for(const ImDrawCmd& cmd : pDrawList->CmdBuffer)
		{
			const uint16_t clipX = std::max<uint16_t>(0, static_cast<uint16_t>(cmd.ClipRect.x));
			const uint16_t clipY = std::max<uint16_t>(0, static_cast<uint16_t>(cmd.ClipRect.y));
			Modules::Render->SetScissor(clipX, clipY, static_cast<uint16_t>(cmd.ClipRect.z - clipX), static_cast<uint16_t>(cmd.ClipRect.w - clipY));

			if(cmd.UserCallback)
			{
				cmd.UserCallback(pDrawList, &cmd);
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

void ImGuiUI::BeginWindow(const String& label)
{
	ImGui::Begin(label.Str());
}

void ImGuiUI::EndWindow()
{
	ImGui::End();
}

void ImGuiUI::Text(const String& label)
{
	ImGui::TextUnformatted(label.Str(), label.Str() + label.Length());
}

bool ImGuiUI::Button(const String& label, const Vector2& size)
{
	return ImGui::Button(label.Str(), ImVec2(size.GetX(), size.GetY()));
}

bool ImGuiUI::RadioButton(const String& label, bool isActive)
{
	return ImGui::RadioButton(label.Str(), isActive);
}

bool ImGuiUI::InputField(const String& label, float& value)
{
	return ImGui::InputFloat(label.Str(), &value, 0.01f, 0.5f);
}

bool ImGuiUI::InputField(const String& label, Vector3& value)
{
	float elements[3];
	value.StoreXYZ(elements);
	if(ImGui::InputFloat3(label.Str(), elements))
	{
		value.LoadXYZ(elements);
		return true;
	}

	return false;
}

bool ImGuiUI::Slider(const String& label, int32_t& value, int32_t minValue, int32_t maxValue)
{
	return ImGui::SliderInt(label.Str(), &value, minValue, maxValue);
}

bool ImGuiUI::Slider(const String& label, float& value, float minValue, float maxValue)
{
	return ImGui::SliderFloat(label.Str(), &value, minValue, maxValue);
}

bool ImGuiUI::Checkbox(const String& label, bool& isChecked)
{
	return ImGui::Checkbox(label.Str(), &isChecked);
}

void ImGuiUI::SameLine()
{
	ImGui::SameLine();
}