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
#include "Engine/Render/Mesh.h"
#include "Engine/Render/Render.h"
#include "Engine/Render/Renderable.h"
#include "Engine/Render/Shader.h"
#include "Engine/Render/Texture.h"

#include "Engine/Resource/ResourceManager.h"

static int s_widgetCounter = 0;

ImGuiUI::ImGuiUI()
{
	ImGui::SetAllocatorFunctions(&ImGuiCallbacks::MallocWrapper, &ImGuiCallbacks::FreeWrapper, this);
}

ImGuiUI::~ImGuiUI()
{
}

void ImGuiUI::Init()
{
	ASSERT(ImGui::GetCurrentContext() == nullptr && "ImGui is already initialized");

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
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

	io.ConfigFlags = ImGuiConfigFlags_DockingEnable;
	io.IniFilename = nullptr;
	io.ConfigWindowsMoveFromTitleBarOnly = true;

	renderable = std::make_unique<Renderable>();

	// Dynamic mesh for the draw list
	const std::initializer_list<Mesh::VertexLayout> vertexLayout =
	{
		{ Mesh::EAttrib::Position,	2, Mesh::EAttribType::Float },
		{ Mesh::EAttrib::Texcoord0,	2, Mesh::EAttribType::Float },
		{ Mesh::EAttrib::Color,		4, Mesh::EAttribType::Uint8 }
	};
	renderable->m_mesh = std::make_unique<DynamicMesh>(vertexLayout, 128 * 1024, 256 * 1024);
	ASSERT(sizeof(ImDrawIdx) == renderable->m_mesh->GetIndexSize() && "ImDrawIdx has changed");
	ASSERT(sizeof(ImDrawVert) == renderable->m_mesh->GetVertexSize() && "ImDrawVert has changed");

	// Setup material
	renderable->m_material = std::make_unique<Material>();
	renderable->m_material->m_vertexShader = Modules::ResourceManager->GetShader("vs_ocornut_imgui");
	renderable->m_material->m_pixelShader = Modules::ResourceManager->GetShader("fs_ocornut_imgui");
	renderable->m_material->m_cullMode = ECullMode::None;
	renderable->m_material->m_isBlendEnabled = true;
	renderable->m_material->m_isDepthWriteEnabled = false;

	// Setup fonts
	io.Fonts->AddFontDefault();

	// Setup font texture
	uint8_t* pTextureData = nullptr;
	int width = 0;
	int height = 0;
	io.Fonts->GetTexDataAsRGBA32(&pTextureData, &width, &height);
	fontTexture = std::make_shared<Texture>(static_cast<uint16_t>(width), static_cast<uint16_t>(height), Texture::EFormat::RGBA8, pTextureData);
	renderable->m_material->AddTexture(fontTexture);
	io.Fonts->TexID = &fontTexture;

	// Cleanup (don't clear the input data if you want to append new fonts later)
	io.Fonts->ClearInputData();
	io.Fonts->ClearTexData();
}

void ImGuiUI::Kill()
{
	ImGui::DestroyContext();

	fontTexture = nullptr;
	renderable = nullptr;
}

void ImGuiUI::NewFrame(OSWindow& window)
{
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(window.GetWidth(), window.GetHeight());
	io.DeltaTime = std::max(FLT_MIN, Modules::Application->GetTimer()->GetSecDelta());		// imgui needs a positive delta time

	// Keyboard
	io.KeyCtrl = Modules::Input->IsKeyDown(KEY_LCONTROL, EKeyModifier::DontCare) || Modules::Input->IsKeyDown(KEY_RCONTROL, EKeyModifier::DontCare);
	io.KeyShift = Modules::Input->IsKeyDown(KEY_LSHIFT, EKeyModifier::DontCare) || Modules::Input->IsKeyDown(KEY_RSHIFT, EKeyModifier::DontCare);
	io.KeyAlt = Modules::Input->IsKeyDown(KEY_LALT, EKeyModifier::DontCare) || Modules::Input->IsKeyDown(KEY_RALT, EKeyModifier::DontCare);
	io.KeySuper = Modules::Input->IsKeyDown(KEY_LWIN, EKeyModifier::DontCare) || Modules::Input->IsKeyDown(KEY_RWIN, EKeyModifier::DontCare);
	for(int keyCode : io.KeyMap)
	{
		io.KeysDown[keyCode] = Modules::Input->IsKeyDown(static_cast<EKeyCode>(keyCode), EKeyModifier::DontCare);
	}

	const String& typedCharacters = Modules::Input->GetTypingCharacters();
	for(size_t i = 0; i < typedCharacters.Length(); ++i)
	{
		io.AddInputCharacter(typedCharacters.Str()[i]);
	}

	// Mouse
	const Vector2& mousePos = Modules::Input->GetOsMouseCursorPos();
	io.MousePos = ImVec2(mousePos.GetX(), mousePos.GetY());
	io.MouseDown[ImGuiMouseButton_Left] = Modules::Input->IsMouseButtonDown(Mouse::EButton::Button_Left);
	io.MouseDown[ImGuiMouseButton_Right] = Modules::Input->IsMouseButtonDown(Mouse::EButton::Button_Right);
	io.MouseDown[ImGuiMouseButton_Middle] = Modules::Input->IsMouseButtonDown(Mouse::EButton::Button_Middle);
	io.MouseWheel = Modules::Input->GetMouseAxisDelta(Mouse::Wheel);
	io.MouseWheelH = Modules::Input->GetMouseAxisDelta(Mouse::Wheel_Horizontal);

	switch(ImGui::GetMouseCursor())
	{
		case ImGuiMouseCursor_Arrow:		window.SetCursor(OSWindow::CURSOR_ARROW); break;
		case ImGuiMouseCursor_TextInput:	window.SetCursor(OSWindow::CURSOR_IBEAM); break;
		case ImGuiMouseCursor_ResizeAll:	window.SetCursor(OSWindow::CURSOR_MOVE); break;
		case ImGuiMouseCursor_ResizeNS:		window.SetCursor(OSWindow::CURSOR_SIZE_NS); break;
		case ImGuiMouseCursor_ResizeEW:		window.SetCursor(OSWindow::CURSOR_SIZE_WE); break;
		case ImGuiMouseCursor_ResizeNESW:	window.SetCursor(OSWindow::CURSOR_SIZE_BOTTOMLEFT); break;
		case ImGuiMouseCursor_ResizeNWSE:	window.SetCursor(OSWindow::CURSOR_SIZE_BOTTOMRIGHT); break;
		case ImGuiMouseCursor_Hand:			window.SetCursor(OSWindow::CURSOR_HAND); break;
		case ImGuiMouseCursor_NotAllowed:	window.SetCursor(OSWindow::CURSOR_NOT_ALLOWED); break;
	};

	ImGui::NewFrame();
	s_widgetCounter = 0;

	// Docking
	if(io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(window.GetWidth(), window.GetHeight()));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("##Docking", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoDocking);
		ImGui::PopStyleVar(2);

		ImGui::PushID(s_widgetCounter++);
		ImGui::DockSpace(ImGui::GetID(""), ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
		ImGui::PopID();

		ImGui::End();
	}
}

void ImGuiUI::Render()
{
	ImGui::Render();

	ImDrawData* pDrawData = ImGui::GetDrawData();
	if(pDrawData == nullptr)
	{
		return;
	}

	// Update mesh with packed buffers for contiguous indices and vertices
	if(static_cast<uint32_t>(pDrawData->TotalIdxCount) > renderable->m_mesh->GetNumIndices() ||
		static_cast<uint32_t>(pDrawData->TotalVtxCount) > renderable->m_mesh->GetNumVertices())
	{
		// Skip rendering if we have too many indices or vertices
		ASSERT(false && "DynamicMesh is too small");
		return;
	}

	ImDrawIdx* pIndex = renderable->m_mesh->m_indices.get();
	ImDrawVert* pVertex = reinterpret_cast<ImDrawVert*>(renderable->m_mesh->m_vertices.get());
	for(int i = 0; i < pDrawData->CmdListsCount; i++)
	{
		const ImDrawList* pDrawList = pDrawData->CmdLists[i];
		memcpy(pIndex, pDrawList->IdxBuffer.Data, pDrawList->IdxBuffer.Size * sizeof(ImDrawIdx));
		memcpy(pVertex, pDrawList->VtxBuffer.Data, pDrawList->VtxBuffer.Size * sizeof(ImDrawVert));
		pIndex += pDrawList->IdxBuffer.Size;
		pVertex += pDrawList->VtxBuffer.Size;
	}

	static_cast<DynamicMesh*>(renderable->m_mesh.get())->m_isIndicesDirty = true;
	static_cast<DynamicMesh*>(renderable->m_mesh.get())->m_isVerticesDirty = true;

	// Because we merged all buffers into a single one, we maintain our own offset into them
	uint32_t indexOffset = 0;
	uint32_t vertexOffset = 0;
	for(int i = 0; i < pDrawData->CmdListsCount; ++i)
	{
		const ImDrawList* pDrawList = pDrawData->CmdLists[i];
		for(const ImDrawCmd& cmd : pDrawList->CmdBuffer)
		{
			if(cmd.UserCallback != nullptr)
			{
				if(cmd.UserCallback == ImDrawCallback_ResetRenderState)
				{
					//  Special callback value used by the user to request the renderer to reset render state
				}
				else
				{
					cmd.UserCallback(pDrawList, &cmd);
				}
			}
			else
			{
				if(cmd.ElemCount == 0)
				{
					continue;
				}

				renderable->m_material->ChangeTexture(0, *static_cast<const std::shared_ptr<Texture>*>(cmd.TextureId));

				const uint16_t x = static_cast<uint16_t>(std::max(0.0f, cmd.ClipRect.x));
				const uint16_t y = static_cast<uint16_t>(std::max(0.0f, cmd.ClipRect.y));
				const uint16_t w = static_cast<uint16_t>(std::min<float>(cmd.ClipRect.z - cmd.ClipRect.x, UINT16_MAX));
				const uint16_t h = static_cast<uint16_t>(std::min<float>(cmd.ClipRect.w - cmd.ClipRect.y, UINT16_MAX));
				Modules::Render->SetScissor(x, y, w, h);

				Modules::Render->RenderRenderable(*renderable, cmd.IdxOffset + indexOffset, cmd.VtxOffset + vertexOffset, cmd.ElemCount);
			}
		}

		indexOffset += pDrawList->IdxBuffer.Size;
		vertexOffset += pDrawList->VtxBuffer.Size;
	}

	Modules::Render->SetScissor();
}

bool ImGuiUI::BeginWindow(const String& label, const Vector2& initialPos, const Vector2& initialSize, bool* pOpen, EWindowFlags flags)
{
	ImGuiWindowFlags imguiFlags = ImGuiWindowFlags_NoCollapse;
	if(flags & WindowFlags_NoTitleBar)
	{
		imguiFlags |= ImGuiWindowFlags_NoTitleBar;
	}
	if(flags & WindowFlags_NoResize)
	{
		imguiFlags |= ImGuiWindowFlags_NoResize;
	}
	if(flags & WindowFlags_NoMove)
	{
		imguiFlags |= ImGuiWindowFlags_NoMove;
	}
	if(flags & WindowFlags_NoScrollbar)
	{
		imguiFlags |= ImGuiWindowFlags_NoScrollbar;
	}

	ImGui::SetNextWindowPos(ImVec2(initialPos.GetX(), initialPos.GetY()), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(initialSize.GetX(), initialSize.GetY()), ImGuiCond_FirstUseEver);

	const bool isVisible = ImGui::Begin(label.Str(), pOpen, imguiFlags);
	if(isVisible)
	{
		// Auto-focus on top level window on all mouse clicks as well (the default is ImGuiMouseButton_Left)
		ImGuiIO& io = ImGui::GetIO();
		for(int i = ImGuiMouseButton_Right; i < ImGuiMouseButton_COUNT; ++i)
		{
			if(io.MouseClicked[i])
			{
				if(!ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows) && ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows))
				{
					ImGui::SetWindowFocus();
				}

				break;
			}
		}
	}

	return isVisible;
}

void ImGuiUI::EndWindow()
{
	ImGui::End();
}

void ImGuiUI::Text(const String& label)
{
	ImGui::AlignTextToFramePadding();

	ImGui::PushID(s_widgetCounter++);
	ImGui::TextUnformatted(label.Str(), label.Str() + label.Length());
	ImGui::PopID();
	ImGui::NextColumn();
}

bool ImGuiUI::TextInput(WritableString& value)
{
	bool isValueChanged = false;

	constexpr int kMaxSize = 255;
	StackString<kMaxSize> string = value;

	ImGui::PushID(s_widgetCounter++);
	if(ImGui::InputText("", string.AsCharBufferWithSize(kMaxSize - 1), kMaxSize, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue, nullptr, nullptr))
	{
		value.Set(string.Str(), strlen(string.Str()));
		isValueChanged = true;
	}
	ImGui::PopID();
	ImGui::NextColumn();

	return isValueChanged;
}

bool ImGuiUI::Button(const String& label, const Vector2& size)
{
	ImGui::PushID(s_widgetCounter++);
	const bool isPressed = ImGui::Button(label.Str(), ImVec2(size.GetX(), size.GetY()));
	ImGui::PopID();
	ImGui::NextColumn();

	return isPressed;
}

bool ImGuiUI::ArrowButton(EArrowDir dir)
{
	bool isPressed = false;

	ImGui::PushID(s_widgetCounter++);
	switch(dir)
	{
		case EArrowDir::Left:	isPressed = ImGui::ArrowButton("", ImGuiDir_Left); break;
		case EArrowDir::Right:	isPressed = ImGui::ArrowButton("", ImGuiDir_Right); break;
		case EArrowDir::Up:		isPressed = ImGui::ArrowButton("", ImGuiDir_Up); break;
		case EArrowDir::Down:	isPressed = ImGui::ArrowButton("", ImGuiDir_Down); break;
	}
	ImGui::PopID();
	ImGui::NextColumn();

	return isPressed;
}

bool ImGuiUI::Checkbox(bool& isChecked, const String& label)
{
	ImGui::PushID(s_widgetCounter++);
	const bool isPressed = ImGui::Checkbox(label.Str(), &isChecked);
	ImGui::PopID();
	ImGui::NextColumn();

	return isPressed;
}

bool ImGuiUI::RadioButtonOption(const String& label, bool isActive)
{
	ImGui::PushID(s_widgetCounter++);
	const bool isPressed = ImGui::RadioButton(label.Str(), isActive) && !isActive;
	ImGui::PopID();
	ImGui::NextColumn();

	return isPressed;
}

bool ImGuiUI::ValueSpinner(int32_t& value, int32_t minValue, int32_t maxValue, float speed)
{
	ImGui::PushID(s_widgetCounter++);
	const bool isValueChanged = ImGui::DragScalar("", ImGuiDataType_S32, &value, speed, &minValue, &maxValue, "%d", ImGuiSliderFlags_AlwaysClamp);
	ImGui::PopID();
	ImGui::NextColumn();

	if(ImGui::IsItemActive())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
	}

	return isValueChanged;
}

bool ImGuiUI::ValueSpinner(float& value, float minValue, float maxValue, float speed)
{
	ImGui::PushID(s_widgetCounter++);
	const bool isValueChanged = ImGui::DragScalar("", ImGuiDataType_Float, &value, speed, &minValue, &maxValue, "%.3f", ImGuiSliderFlags_AlwaysClamp);
	ImGui::PopID();
	ImGui::NextColumn();

	if(ImGui::IsItemActive())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
	}

	return isValueChanged;
}

bool ImGuiUI::ValueEdit(float& value)
{
	ImGui::PushID(s_widgetCounter++);
	const bool isValueChanged = ImGui::InputScalar("", ImGuiDataType_Float, &value, nullptr, nullptr, "%.3f", ImGuiInputTextFlags_CharsDecimal);
	ImGui::PopID();
	ImGui::NextColumn();

	return isValueChanged;
}

bool ImGuiUI::ValueEdit(Vector3& value)
{
	bool isValueChanged = false;

	float elements[3];
	value.StoreXYZ(elements);

	ImGui::PushID(s_widgetCounter++);
	if(ImGui::InputScalarN("", ImGuiDataType_Float, elements, 3, nullptr, nullptr, "%.3f", ImGuiInputTextFlags_CharsDecimal))
	{
		value.LoadXYZ(elements);
		isValueChanged = true;
	}
	ImGui::PopID();
	ImGui::NextColumn();

	return isValueChanged;
}

bool ImGuiUI::Slider(int32_t& value, int32_t minValue, int32_t maxValue)
{
	ImGui::PushID(s_widgetCounter++);
	const bool isValueChanged = ImGui::SliderScalar("", ImGuiDataType_S32 , &value, &minValue, &maxValue, "%d", ImGuiSliderFlags_AlwaysClamp);
	ImGui::PopID();
	ImGui::NextColumn();

	return isValueChanged;
}

bool ImGuiUI::Slider(float& value, float minValue, float maxValue)
{
	ImGui::PushID(s_widgetCounter++);
	const bool isValueChanged = ImGui::SliderScalar("", ImGuiDataType_Float, &value, &minValue, &maxValue, "%.3f", ImGuiSliderFlags_AlwaysClamp);
	ImGui::PopID();
	ImGui::NextColumn();

	return isValueChanged;
}

void ImGuiUI::ProgressBar(float progress)
{
	ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f), nullptr);
	ImGui::NextColumn();
}

bool ImGuiUI::ColorPicker(float(&colorRGB)[3])
{
	ImGui::PushID(s_widgetCounter++);
	const bool isValueChanged = ImGui::ColorEdit3("", colorRGB, ImGuiColorEditFlags_None);
	ImGui::PopID();
	ImGui::NextColumn();

	return isValueChanged;
}

bool ImGuiUI::ColorPicker(float(&colorRGBA)[4])
{
	ImGui::PushID(s_widgetCounter++);
	const bool isValueChanged = ImGui::ColorEdit4("", colorRGBA, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
	ImGui::PopID();
	ImGui::NextColumn();

	return isValueChanged;
}

void ImGuiUI::LayoutColumns(uint8_t numColumns, const float* pRatio)
{
	StackString<16> strId;
	strId += s_widgetCounter++;

	ImGuiStyle& style = ImGui::GetStyle();
	const float itemSpacingX = style.ItemSpacing.x;
	style.ItemSpacing.x = 0.0f;
	ImGui::Columns(1);
	ImGui::Columns(numColumns, strId.Str(), false);
	style.ItemSpacing.x = itemSpacingX;

	if(pRatio != nullptr)
	{
		const float width = ImGui::GetWindowWidth();
		for(uint8_t i = 0; i < numColumns; ++i)
		{
			ImGui::SetColumnWidth(i, width * pRatio[i]);
		}
	}
}
