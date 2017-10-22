#include "stdafx.h"
#include "UI/ImGuiImpl.h"
#include "UI/fs_ocornut_imgui.bin.h"
#include "UI/vs_ocornut_imgui.bin.h"
#include "Core/platform.h"
#include "Core/IWindow.h"
#include "Core/Timer.h"
#include "Input/Input.h"
#include "Render/Camera.h"
#include "Render/IRender.h"
#include "Resource/Mesh.h"
#include "Resource/Texture.h"

#include "UI-imgui/include/imgui.h"

/**/#include "bgfx/bgfx.h"
/**/#include "Render/bgfx/MeshRenderDataBgfx.h"
/**/bgfx::ProgramHandle imguiShaderProgram;

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

	// Setup font texture
	unsigned char *textureData;
	int width, height;
	io.Fonts->AddFontDefault();
	io.Fonts->GetTexDataAsRGBA32(&textureData, &width, &height);

	fontTexture = std::make_unique<Texture>((uint16_t)width, (uint16_t)height, Texture::EFormat::RGBA8);
	fontTexture->data = SMemoryBuffer::CreateFromDataCopy(textureData, width * height * 4u);
	io.Fonts->TexID = fontTexture.get();

	// Cleanup (don't clear the input data if you want to append new fonts later)
	io.Fonts->ClearInputData();
	io.Fonts->ClearTexData();

	// Transient mesh for the draw list
	mesh = std::make_unique<Mesh>(Mesh::EType::TRANSIENT);
	mesh->stride = 2 * sizeof(float) + 2 * sizeof(float) + 4 * sizeof(uint8_t);

	// Setup shader
	const bgfx::Memory *vsmem = nullptr;
	const bgfx::Memory *fsmem = nullptr;
	switch(bgfx::getRendererType())
	{
		case bgfx::RendererType::Direct3D11:
		case bgfx::RendererType::Direct3D12:
			vsmem = bgfx::makeRef(vs_ocornut_imgui_dx11, sizeof(vs_ocornut_imgui_dx11));
			fsmem = bgfx::makeRef(fs_ocornut_imgui_dx11, sizeof(fs_ocornut_imgui_dx11));
			break;

		case bgfx::RendererType::Metal:
			vsmem = bgfx::makeRef(vs_ocornut_imgui_mtl, sizeof(vs_ocornut_imgui_mtl));
			fsmem = bgfx::makeRef(fs_ocornut_imgui_mtl, sizeof(fs_ocornut_imgui_mtl));
			break;

		case bgfx::RendererType::Vulkan:
			break;

		default:
			ASSERT(false);
			return;
	}

	bgfx::ShaderHandle vsh = bgfx::createShader(vsmem);
	bgfx::ShaderHandle fsh = bgfx::createShader(fsmem);
	imguiShaderProgram = bgfx::createProgram(vsh, fsh, true);
}

void ImGuiImpl::Kill()
{
	ImGui::Shutdown();

	bgfx::destroyProgram(imguiShaderProgram);

	IRender::Get()->Unbind(mesh.get());
	mesh = nullptr;

	IRender::Get()->Unbind(fontTexture.get());
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

	const SPoint<int16_t>& mousePos = Input::Get()->GetOsMouseCursorPos();
	io.MousePos = ImVec2(mousePos.x, mousePos.y);

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

void ImGuiImpl::Render(ImDrawData *drawData)
{
	ImGuiIO& io = ImGui::GetIO();
	IRender *render = IRender::Get();

	render->SetViewport(0, 0, (uint16_t)io.DisplaySize.x, (uint16_t)io.DisplaySize.y);

	float ortho[16];
	Matrix4 matOrtho = Camera::CreateOrthographicProjectonMatrix(io.DisplaySize.x, io.DisplaySize.y, -1.0f, 1.0f);
	matOrtho.col0.StoreXYZW(&ortho[0]);
	matOrtho.col1.StoreXYZW(&ortho[4]);
	matOrtho.col2.StoreXYZW(&ortho[8]);
	matOrtho.col3.StoreXYZW(&ortho[12]);

	const uint8_t viewId = 0;
	bgfx::setViewTransform(viewId, nullptr, ortho);

	// Resize buffers if needed
	if(mesh->numIndices != (uint32_t)drawData->TotalIdxCount)
	{
		mesh->numIndices = drawData->TotalIdxCount;
		mesh->indices = (uint16_t*)realloc(mesh->indices, mesh->GetSizeOfIndices());
	}

	if(mesh->numVertices != (uint32_t)drawData->TotalVtxCount)
	{
		mesh->numVertices = drawData->TotalVtxCount;
		mesh->vertices = (float*)realloc(mesh->vertices, mesh->GetSizeOfVertices());
	}

	ASSERT(sizeof(ImDrawIdx) == sizeof(uint16_t) && "Conversion is required for index buffer");
	ASSERT(sizeof(ImDrawVert) == mesh->stride && "ImDrawVert struct layout has changed");
	uint8_t *indicesPtr = reinterpret_cast<uint8_t*>(mesh->indices);
	uint8_t *verticesPtr = reinterpret_cast<uint8_t*>(mesh->vertices);
	for(int i = 0; i < drawData->CmdListsCount; ++i)
	{
		const ImDrawList *drawList = drawData->CmdLists[i];
		const size_t sizeOfIndices = (size_t)drawList->IdxBuffer.size() * sizeof(ImDrawIdx);
		ASSERT(indicesPtr + sizeOfIndices <= reinterpret_cast<uint8_t*>(mesh->indices) + mesh->GetSizeOfIndices());
		memcpy(indicesPtr, drawList->IdxBuffer.begin(), sizeOfIndices);

		const size_t sizeOfVertices = (size_t)drawList->VtxBuffer.size() * sizeof(ImDrawVert);
		ASSERT(verticesPtr + sizeOfVertices <= reinterpret_cast<uint8_t*>(mesh->vertices) + mesh->GetSizeOfVertices());
		memcpy(verticesPtr, drawList->VtxBuffer.begin(), sizeOfVertices);

		indicesPtr += sizeOfIndices;
		verticesPtr += sizeOfVertices;
	}
	mesh->isUpdateRequired = true;

	mesh->indexOffset = 0;
	mesh->vertexOffset = 0;
	for(int i = 0; i < drawData->CmdListsCount; ++i)
	{
		const ImDrawList *drawList = drawData->CmdLists[i];

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

			const uint16_t clipX = (uint16_t)std::fmax(pcmd.ClipRect.x, 0.0f);
			const uint16_t clipY = (uint16_t)std::fmax(pcmd.ClipRect.y, 0.0f);
			render->SetScissor(clipX, clipY, (uint16_t)(pcmd.ClipRect.z - clipX), (uint16_t)(pcmd.ClipRect.w - clipY));

			bgfx::setState(0
				| BGFX_STATE_RGB_WRITE
				| BGFX_STATE_ALPHA_WRITE
				| BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA)
				| BGFX_STATE_MSAA
			);

			render->SetMesh(mesh.get());
			MeshRenderDataBgfx *renderData = static_cast<MeshRenderDataBgfx*>(mesh->renderData);
			bgfx::setIndexBuffer(&renderData->transientIndexBuffer, mesh->indexOffset, pcmd.ElemCount);
			bgfx::setVertexBuffer(&renderData->transientVertexBuffer, mesh->vertexOffset, drawList->VtxBuffer.size());

			render->SetTexture(0, static_cast<Texture*>(pcmd.TextureId));

			bgfx::submit(viewId, imguiShaderProgram);

			mesh->indexOffset += pcmd.ElemCount;
		}

		mesh->vertexOffset += drawList->VtxBuffer.size();
	}
}

void ImGuiImpl::RenderDrawListsCallback(ImDrawData *data)
{
	ImGuiImpl::Get()->Render(data);
}
