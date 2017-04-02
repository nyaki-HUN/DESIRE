#include "stdafx.h"
#include "UI/ImGuiImpl.h"
#include "UI/fs_ocornut_imgui.bin.h"
#include "UI/vs_ocornut_imgui.bin.h"
#include "Core/platform.h"
#include "Core/IWindow.h"
#include "Core/Timer.h"
#include "Input/Input.h"
#include "Render/Camera.h"

#include "UI-imgui/include/imgui.h"

ImGuiImpl::ImGuiImpl()
{

}

ImGuiImpl::~ImGuiImpl()
{

}

void ImGuiImpl::Init()
{
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
	io.KeyMap[ImGuiKey_Delete] = KEY_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = KEY_BACKSPACE;
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

//	ImGuiStyle& style = ImGui::GetStyle();
//	style.FrameRounding = 4.0f;

	// Setup font texture
	unsigned char *data;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&data, &width, &height);

	fontTexture = bgfx::createTexture2D((uint16_t)width, (uint16_t)height, false, 1, bgfx::TextureFormat::BGRA8, BGFX_TEXTURE_NONE, bgfx::copy(data, width * height * 4u));
	io.Fonts->TexID = &fontTexture;

	// Cleanup (don't clear the input data if you want to append new fonts later)
	io.Fonts->ClearInputData();
	io.Fonts->ClearTexData();

	// Setup shader
	const bgfx::Memory *vsmem = nullptr;
	const bgfx::Memory *fsmem = nullptr;
	switch(bgfx::getRendererType())
	{
		case bgfx::RendererType::Noop:
			break;

		case bgfx::RendererType::Direct3D9:
			vsmem = bgfx::makeRef(vs_ocornut_imgui_dx9, sizeof(vs_ocornut_imgui_dx9));
			fsmem = bgfx::makeRef(fs_ocornut_imgui_dx9, sizeof(fs_ocornut_imgui_dx9));
			break;

		case bgfx::RendererType::Direct3D11:
		case bgfx::RendererType::Direct3D12:
			vsmem = bgfx::makeRef(vs_ocornut_imgui_dx11, sizeof(vs_ocornut_imgui_dx11));
			fsmem = bgfx::makeRef(fs_ocornut_imgui_dx11, sizeof(fs_ocornut_imgui_dx11));
			break;

		case bgfx::RendererType::Gnm:
			break;

		case bgfx::RendererType::Metal:
			vsmem = bgfx::makeRef(vs_ocornut_imgui_mtl, sizeof(vs_ocornut_imgui_mtl));
			fsmem = bgfx::makeRef(fs_ocornut_imgui_mtl, sizeof(fs_ocornut_imgui_mtl));
			break;

		case bgfx::RendererType::OpenGLES:
		case bgfx::RendererType::OpenGL:
			vsmem = bgfx::makeRef(vs_ocornut_imgui_glsl, sizeof(vs_ocornut_imgui_glsl));
			fsmem = bgfx::makeRef(fs_ocornut_imgui_glsl, sizeof(fs_ocornut_imgui_glsl));

		case bgfx::RendererType::Vulkan:
			break;

		case bgfx::RendererType::Count:
			ASSERT(false);
			break;
	}

	bgfx::ShaderHandle vsh = bgfx::createShader(vsmem);
	bgfx::ShaderHandle fsh = bgfx::createShader(fsmem);
	shaderProgram = bgfx::createProgram(vsh, fsh, true);

	m_decl.begin()
		.add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
		.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
		.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
		.end();

	s_tex = bgfx::createUniform("s_tex", bgfx::UniformType::Int1);
}

void ImGuiImpl::Kill()
{
	ImGui::Shutdown();

	bgfx::destroyUniform(s_tex);
	bgfx::destroyProgram(shaderProgram);
	bgfx::destroyTexture(fontTexture);
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
			io.KeysDown[keyCode] = keyboard.IsDown((EKeyCode)keyCode);
		}
	}

	const char *typedCharacters = Input::Get()->GetTypingCharacters();
	for(int i = 0; typedCharacters[i] != '\0'; ++i)
	{
		io.AddInputCharacter((ImWchar)typedCharacters[i]);
	}

	// Mouse
	const std::vector<Mouse>& mouses = Input::Get()->GetMouses();
	if(!mouses.empty())
	{
		const Mouse& mouse = mouses.back();
		io.MouseWheel = mouse.wheelDelta;
		for(int i = 0; i < DESIRE_ASIZEOF(io.MouseDown); ++i)
		{
			io.MouseDown[i] = mouse.IsDown((Mouse::EButton)i);
		}

		const SPoint<int16_t>& mousePos = Input::Get()->GetOsMouseCursorPos();
		io.MousePos = ImVec2(mousePos.x, mousePos.y);
	}
	else
	{
		io.MousePos = ImVec2(-1, -1);
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

void ImGuiImpl::Render(ImDrawData *data)
{
	ImGuiIO& io = ImGui::GetIO();
	uint8_t viewId = 0;

	const float width = io.DisplaySize.x;
	const float height = io.DisplaySize.y;

	float ortho[16];
	Matrix4 matOrtho = Camera::CreateOrthographicProjectonMatrix(width, height, -1.0f, 1.0f);
	matOrtho.col0.StoreXYZW(&ortho[0]);
	matOrtho.col1.StoreXYZW(&ortho[4]);
	matOrtho.col2.StoreXYZW(&ortho[8]);
	matOrtho.col3.StoreXYZW(&ortho[12]);

	bgfx::setViewTransform(viewId, nullptr, ortho);

	for(int i = 0; i < data->CmdListsCount; ++i)
	{
		const ImDrawList *drawList = data->CmdLists[i];

		const uint32_t numIndices = (uint32_t)drawList->IdxBuffer.size();
		const uint32_t numVertices = (uint32_t)drawList->VtxBuffer.size();

		if(bgfx::getAvailTransientIndexBuffer(numIndices) != numIndices || bgfx::getAvailTransientVertexBuffer(numVertices, m_decl) != numVertices)
		{
			LOG_WARNING("Not enough space in transient buffer");
			break;
		}

		bgfx::TransientIndexBuffer tib;
		bgfx::TransientVertexBuffer tvb;
		bgfx::allocTransientIndexBuffer(&tib, numIndices);
		bgfx::allocTransientVertexBuffer(&tvb, numVertices, m_decl);

		ImDrawIdx *indices = (ImDrawIdx*)tib.data;
		ImDrawVert *vertices = (ImDrawVert*)tvb.data;
		memcpy(indices, drawList->IdxBuffer.begin(), numIndices * sizeof(ImDrawIdx));
		memcpy(vertices, drawList->VtxBuffer.begin(), numVertices * sizeof(ImDrawVert));

		uint32_t indexOffset = 0;
		for(const ImDrawCmd& pcmd : drawList->CmdBuffer)
		{
			if(pcmd.UserCallback)
			{
				pcmd.UserCallback(drawList, &pcmd);
				continue;
			}

			if(pcmd.ElemCount == 0)
			{
				continue;
			}

			bgfx::setState(0
				| BGFX_STATE_RGB_WRITE
				| BGFX_STATE_ALPHA_WRITE
				| BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA)
				| BGFX_STATE_MSAA
			);

			const uint16_t clipX = (uint16_t)fmaxf(pcmd.ClipRect.x, 0.0f);
			const uint16_t clipY = (uint16_t)fmaxf(pcmd.ClipRect.y, 0.0f);
			bgfx::setScissor(clipX, clipY, (uint16_t)(pcmd.ClipRect.z - clipX), (uint16_t)(pcmd.ClipRect.w - clipY));

			bgfx::setIndexBuffer(&tib, indexOffset, pcmd.ElemCount);
			bgfx::setVertexBuffer(&tvb, 0, numVertices);
			bgfx::setTexture(0, s_tex, *(bgfx::TextureHandle*)pcmd.TextureId);
			bgfx::submit(viewId, shaderProgram);

			indexOffset += pcmd.ElemCount;
		}
	}
}

void ImGuiImpl::RenderDrawListsCallback(ImDrawData *data)
{
	ImGuiImpl::Get()->Render(data);
}
