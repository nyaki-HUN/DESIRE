#include "stdafx_imgui.h"
#include "ImGuiUI.h"

#include "ImGuiCallbacks.h"

#include "Engine/Application/Application.h"
#include "Engine/Application/OSWindow.h"
#include "Engine/Application/ResourceManager.h"

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

	m_spRenderable = std::make_unique<Renderable>();

	// Dynamic mesh for the draw list
	const std::initializer_list<Mesh::VertexLayout> vertexLayout =
	{
		{ Mesh::EAttrib::Position,	2, Mesh::EAttribType::Float },
		{ Mesh::EAttrib::Texcoord0,	2, Mesh::EAttribType::Float },
		{ Mesh::EAttrib::Color,		4, Mesh::EAttribType::Uint8 }
	};
	m_spRenderable->m_spMesh = std::make_unique<DynamicMesh>(vertexLayout, 128 * 1024, 256 * 1024);
	ASSERT(sizeof(ImDrawIdx) == m_spRenderable->m_spMesh->GetIndexSize() && "ImDrawIdx has changed");
	ASSERT(sizeof(ImDrawVert) == m_spRenderable->m_spMesh->GetVertexSize() && "ImDrawVert has changed");

	// Setup material
	m_spRenderable->m_spMaterial = std::make_unique<Material>();
	m_spRenderable->m_spMaterial->m_spVertexShader = Modules::Application->GetResourceManager().GetShader("vs_ocornut_imgui");
	m_spRenderable->m_spMaterial->m_spPixelShader = Modules::Application->GetResourceManager().GetShader("fs_ocornut_imgui");
	m_spRenderable->m_spMaterial->m_cullMode = ECullMode::None;
	m_spRenderable->m_spMaterial->m_isBlendEnabled = true;
	m_spRenderable->m_spMaterial->m_isDepthWriteEnabled = false;

	// Setup fonts
	io.Fonts->AddFontDefault();

	// Setup font texture
	uint8_t* pTextureData = nullptr;
	int width = 0;
	int height = 0;
	io.Fonts->GetTexDataAsRGBA32(&pTextureData, &width, &height);
	m_spFontTexture = std::make_shared<Texture>(static_cast<uint16_t>(width), static_cast<uint16_t>(height), Texture::EFormat::RGBA8, pTextureData);
	m_spRenderable->m_spMaterial->AddTexture(m_spFontTexture);
	io.Fonts->TexID = &m_spFontTexture;

	// Cleanup (don't clear the input data if you want to append new fonts later)
	io.Fonts->ClearInputData();
	io.Fonts->ClearTexData();
}

void ImGuiUI::Kill()
{
	ImGui::DestroyContext();

	m_spFontTexture = nullptr;
	m_spRenderable = nullptr;
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
	if(static_cast<uint32_t>(pDrawData->TotalIdxCount) > m_spRenderable->m_spMesh->GetNumIndices() ||
		static_cast<uint32_t>(pDrawData->TotalVtxCount) > m_spRenderable->m_spMesh->GetNumVertices())
	{
		// Skip rendering if we have too many indices or vertices
		ASSERT(false && "DynamicMesh is too small");
		return;
	}

	ImDrawIdx* pIndex = m_spRenderable->m_spMesh->m_spIndices.get();
	ImDrawVert* pVertex = reinterpret_cast<ImDrawVert*>(m_spRenderable->m_spMesh->m_spVertices.get());
	for(int i = 0; i < pDrawData->CmdListsCount; i++)
	{
		const ImDrawList* pDrawList = pDrawData->CmdLists[i];
		memcpy(pIndex, pDrawList->IdxBuffer.Data, pDrawList->IdxBuffer.Size * sizeof(ImDrawIdx));
		memcpy(pVertex, pDrawList->VtxBuffer.Data, pDrawList->VtxBuffer.Size * sizeof(ImDrawVert));
		pIndex += pDrawList->IdxBuffer.Size;
		pVertex += pDrawList->VtxBuffer.Size;
	}

	static_cast<DynamicMesh*>(m_spRenderable->m_spMesh.get())->m_isIndicesDirty = true;
	static_cast<DynamicMesh*>(m_spRenderable->m_spMesh.get())->m_isVerticesDirty = true;

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

				m_spRenderable->m_spMaterial->ChangeTexture(0, *static_cast<const std::shared_ptr<Texture>*>(cmd.TextureId));

				const uint16_t x = static_cast<uint16_t>(std::max(0.0f, cmd.ClipRect.x));
				const uint16_t y = static_cast<uint16_t>(std::max(0.0f, cmd.ClipRect.y));
				const uint16_t w = static_cast<uint16_t>(std::min<float>(cmd.ClipRect.z - cmd.ClipRect.x, UINT16_MAX));
				const uint16_t h = static_cast<uint16_t>(std::min<float>(cmd.ClipRect.w - cmd.ClipRect.y, UINT16_MAX));
				Modules::Render->SetScissor(x, y, w, h);

				Modules::Render->RenderRenderable(*m_spRenderable, cmd.IdxOffset + indexOffset, cmd.VtxOffset + vertexOffset, cmd.ElemCount);
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

bool ImGuiUI::BeginTable(const String& id, uint8_t numColumns, const float* pInitialColumnsRatio)
{
	if(ImGui::TableNextColumn())
	{
		ImGui::SetNextItemWidth(-FLT_MIN);
	}

	bool isVisible = ImGui::BeginTable(id.Str(), numColumns, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBodyUntilResize);
	if(isVisible)
	{
		if(pInitialColumnsRatio != nullptr)
		{
			for(uint8_t i = 0; i < numColumns; ++i)
			{
				ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_None, pInitialColumnsRatio[i]);
			}
		}
	}

	return isVisible;
}

void ImGuiUI::EndTable()
{
	ImGui::EndTable();
}

void ImGuiUI::Text(const String& label)
{
	if(ImGui::TableNextColumn())
	{
		ImGui::SetNextItemWidth(-FLT_MIN);
	}

	ImGui::AlignTextToFramePadding();

	ImGui::PushID(s_widgetCounter++);
	ImGui::TextUnformatted(label.Str(), label.Str() + label.Length());
	ImGui::PopID();
}

bool ImGuiUI::TextInput(WritableString& value)
{
	if(ImGui::TableNextColumn())
	{
		ImGui::SetNextItemWidth(-FLT_MIN);
	}

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

	return isValueChanged;
}

bool ImGuiUI::Button(const String& label, const Vector2& size)
{
	if(ImGui::TableNextColumn())
	{
		ImGui::SetNextItemWidth(-FLT_MIN);
	}

	ImGui::PushID(s_widgetCounter++);
	const bool isPressed = ImGui::Button(label.Str(), ImVec2(size.GetX(), size.GetY()));
	ImGui::PopID();

	return isPressed;
}

bool ImGuiUI::ArrowButton(EArrowDir dir)
{
	if(ImGui::TableNextColumn())
	{
		ImGui::SetNextItemWidth(-FLT_MIN);
	}

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

	return isPressed;
}

bool ImGuiUI::Checkbox(bool& isChecked, const String& label)
{
	if(ImGui::TableNextColumn())
	{
		ImGui::SetNextItemWidth(-FLT_MIN);
	}

	ImGui::PushID(s_widgetCounter++);
	const bool isPressed = ImGui::Checkbox(label.Str(), &isChecked);
	ImGui::PopID();

	return isPressed;
}

bool ImGuiUI::RadioButtonOption(const String& label, bool isActive)
{
	if(ImGui::TableNextColumn())
	{
		ImGui::SetNextItemWidth(-FLT_MIN);
	}

	ImGui::PushID(s_widgetCounter++);
	const bool isPressed = ImGui::RadioButton(label.Str(), isActive) && !isActive;
	ImGui::PopID();

	return isPressed;
}

bool ImGuiUI::ValueSpinner(int32_t& value, int32_t minValue, int32_t maxValue, float speed)
{
	if(ImGui::TableNextColumn())
	{
		ImGui::SetNextItemWidth(-FLT_MIN);
	}

	ImGui::PushID(s_widgetCounter++);
	const bool isValueChanged = ImGui::DragScalar("", ImGuiDataType_S32, &value, speed, &minValue, &maxValue, "%d", ImGuiSliderFlags_AlwaysClamp);
	ImGui::PopID();

	if(ImGui::IsItemActive())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
	}

	return isValueChanged;
}

bool ImGuiUI::ValueSpinner(float& value, float minValue, float maxValue, float speed)
{
	if(ImGui::TableNextColumn())
	{
		ImGui::SetNextItemWidth(-FLT_MIN);
	}

	ImGui::PushID(s_widgetCounter++);
	const bool isValueChanged = ImGui::DragScalar("", ImGuiDataType_Float, &value, speed, &minValue, &maxValue, "%.3f", ImGuiSliderFlags_AlwaysClamp);
	ImGui::PopID();

	if(ImGui::IsItemActive())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
	}

	return isValueChanged;
}

bool ImGuiUI::ValueEdit(float& value)
{
	if(ImGui::TableNextColumn())
	{
		ImGui::SetNextItemWidth(-FLT_MIN);
	}

	ImGui::PushID(s_widgetCounter++);
	const bool isValueChanged = ImGui::InputScalar("", ImGuiDataType_Float, &value, nullptr, nullptr, "%.3f", ImGuiInputTextFlags_CharsDecimal);
	ImGui::PopID();

	return isValueChanged;
}

bool ImGuiUI::ValueEdit(Vector3& value)
{
	if(ImGui::TableNextColumn())
	{
		ImGui::SetNextItemWidth(-FLT_MIN);
	}

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

	return isValueChanged;
}

bool ImGuiUI::Slider(int32_t& value, int32_t minValue, int32_t maxValue)
{
	if(ImGui::TableNextColumn())
	{
		ImGui::SetNextItemWidth(-FLT_MIN);
	}

	ImGui::PushID(s_widgetCounter++);
	const bool isValueChanged = ImGui::SliderScalar("", ImGuiDataType_S32 , &value, &minValue, &maxValue, "%d", ImGuiSliderFlags_AlwaysClamp);
	ImGui::PopID();

	return isValueChanged;
}

bool ImGuiUI::Slider(float& value, float minValue, float maxValue)
{
	if(ImGui::TableNextColumn())
	{
		ImGui::SetNextItemWidth(-FLT_MIN);
	}

	ImGui::PushID(s_widgetCounter++);
	const bool isValueChanged = ImGui::SliderScalar("", ImGuiDataType_Float, &value, &minValue, &maxValue, "%.3f", ImGuiSliderFlags_AlwaysClamp);
	ImGui::PopID();

	return isValueChanged;
}

void ImGuiUI::ProgressBar(float progress)
{
	if(ImGui::TableNextColumn())
	{
		ImGui::SetNextItemWidth(-FLT_MIN);
	}

	ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f), nullptr);
}

bool ImGuiUI::ColorPicker(float(&colorRGB)[3])
{
	if(ImGui::TableNextColumn())
	{
		ImGui::SetNextItemWidth(-FLT_MIN);
	}

	ImGui::PushID(s_widgetCounter++);
	const bool isValueChanged = ImGui::ColorEdit3("", colorRGB, ImGuiColorEditFlags_None);
	ImGui::PopID();

	return isValueChanged;
}

bool ImGuiUI::ColorPicker(float(&colorRGBA)[4])
{
	if(ImGui::TableNextColumn())
	{
		ImGui::SetNextItemWidth(-FLT_MIN);
	}

	ImGui::PushID(s_widgetCounter++);
	const bool isValueChanged = ImGui::ColorEdit4("", colorRGBA, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
	ImGui::PopID();

	return isValueChanged;
}

