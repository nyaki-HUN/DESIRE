#pragma once

#include "Engine/Render/Render.h"

#include <d3d12.h>
#include <DirectXMath.h>

class ShaderRenderDataD3D12;
struct IDXGISwapChain1;

class Direct3D12Render : public Render
{
public:
	Direct3D12Render();

	void Init(OSWindow* mainWindow) override;
	void UpdateRenderWindow(OSWindow* window) override;
	void Kill() override;

	void AppendShaderFilenameWithPath(WritableString& outString, const String& shaderFilename) const override;

	void BeginFrame(OSWindow* window) override;
	void EndFrame() override;

	void SetView(View* view) override;

	void SetWorldMatrix(const Matrix4& matrix) override;
	void SetViewProjectionMatrices(const Matrix4& viewMatrix, const Matrix4& projMatrix) override;

	void SetScissor(uint16_t x = 0, uint16_t y = 0, uint16_t width = 0, uint16_t height = 0) override;
	void SetClearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) override;
	void SetColorWriteEnabled(bool r, bool g, bool b, bool a) override;
	void SetDepthWriteEnabled(bool enabled) override;
	void SetDepthTest(EDepthTest deptTest) override;
	void SetCullMode(ECullMode cullMode) override;
	void SetBlendModeSeparated(EBlend srcBlendRGB, EBlend destBlendRGB, EBlendOp blendOpRGB, EBlend srcBlendAlpha, EBlend destBlendAlpha, EBlendOp blendOpAlpha) override;
	void SetBlendModeDisabled() override;

	void Bind(Mesh* mesh) override;
	void Bind(Shader* shader) override;
	void Bind(Texture* texture) override;
	void Bind(RenderTarget* renderTarget) override;

	void Unbind(Mesh* mesh) override;
	void Unbind(Shader* shader) override;
	void Unbind(Texture* texture) override;
	void Unbind(RenderTarget* renderTarget) override;

	void UpdateDynamicMesh(DynamicMesh* mesh) override;

private:
	void SetViewport(uint16_t x, uint16_t y, uint16_t width, uint16_t height) override;

	void SetMesh(Mesh* mesh) override;
	void SetScreenSpaceQuadMesh() override;
	void SetVertexShader(Shader* vertexShader) override;
	void SetFragmentShader(Shader* fragmentShader) override;
	void SetTexture(uint8_t samplerIdx, Texture* texture, EFilterMode filterMode, EAddressMode addressMode) override;
	void UpdateShaderParams(const Material* material) override;
	void UpdateShaderParams(const Material* material, const ShaderRenderDataD3D12* shaderRenderData);
	static bool CheckAndUpdateShaderParam(const void* value, void* valueInConstantBuffer, uint32_t size);

	void DoRender() override;

	static DXGI_FORMAT GetTextureFormat(const Texture* texture);

	ID3D12Device3* d3dDevice = nullptr;
	ID3D12CommandQueue* commandQueue = nullptr;
	IDXGISwapChain1* swapChain = nullptr;

	D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
	D3D12_RASTERIZER_DESC rasterizerDesc = {};
	D3D12_BLEND_DESC blendDesc = {};

	const OSWindow* activeWindow = nullptr;
	const View* activeView = nullptr;
	const Mesh* activeMesh = nullptr;
	const Shader* activeVertexShader = nullptr;
	const Shader* activeFragmentShader = nullptr;

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	DirectX::XMMATRIX matWorld;
	DirectX::XMMATRIX matView;
	DirectX::XMMATRIX matProj;

	std::unique_ptr<Shader> errorVertexShader;
	std::unique_ptr<Shader> errorPixelShader;

	bool initialized = false;
};
