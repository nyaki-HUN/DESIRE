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
	~Direct3D12Render();

	void Init(OSWindow* pMainWindow) override;
	void UpdateRenderWindow(OSWindow* pWindow) override;
	void Kill() override;

	void AppendShaderFilenameWithPath(WritableString& outString, const String& shaderFilename) const override;

	void BeginFrame(OSWindow* pWindow) override;
	void EndFrame() override;

	void SetView(View* pView) override;

	void SetWorldMatrix(const Matrix4& matrix) override;
	void SetViewProjectionMatrices(const Matrix4& viewMatrix, const Matrix4& projMatrix) override;

	void SetScissor(uint16_t x = 0, uint16_t y = 0, uint16_t width = 0, uint16_t height = 0) override;
	void SetClearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) override;
	void SetColorWriteEnabled(bool r, bool g, bool b, bool a) override;
	void SetDepthWriteEnabled(bool enabled) override;
	void SetDepthTest(EDepthTest depthTest) override;
	void SetCullMode(ECullMode cullMode) override;
	void SetBlendModeSeparated(EBlend srcBlendRGB, EBlend destBlendRGB, EBlendOp blendOpRGB, EBlend srcBlendAlpha, EBlend destBlendAlpha, EBlendOp blendOpAlpha) override;
	void SetBlendModeDisabled() override;

	void Bind(Mesh* pMesh) override;
	void Bind(Shader* pShader) override;
	void Bind(Texture* pTexture) override;
	void Bind(RenderTarget* pRenderTarget) override;

	void Unbind(Mesh* pMesh) override;
	void Unbind(Shader* pShader) override;
	void Unbind(Texture* pTexture) override;
	void Unbind(RenderTarget* pRenderTarget) override;

private:
	void SetViewport(uint16_t x, uint16_t y, uint16_t width, uint16_t height) override;

	void SetMesh(Mesh* pMesh) override;
	void SetScreenSpaceQuadMesh() override;
	void SetVertexShader(Shader* pVertexShader) override;
	void SetFragmentShader(Shader* pFragmentShader) override;
	void SetTexture(uint8_t samplerIdx, Texture* pTexture, EFilterMode filterMode, EAddressMode addressMode) override;
	void UpdateShaderParams(const Material* pMaterial) override;
	void UpdateShaderParams(const Material* pMaterial, const ShaderRenderDataD3D12* pShaderRenderData);
	static bool CheckAndUpdateShaderParam(const void* pValue, void* pValueInConstantBuffer, uint32_t size);

	void DoRender() override;

	static DXGI_FORMAT GetTextureFormat(const Texture* pTexture);

	ID3D12Device3* d3dDevice = nullptr;
	ID3D12CommandQueue* commandQueue = nullptr;
	IDXGISwapChain1* swapChain = nullptr;

	D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
	D3D12_RASTERIZER_DESC rasterizerDesc = {};
	D3D12_BLEND_DESC blendDesc = {};

	const OSWindow* pActiveWindow = nullptr;
	const View* pActiveView = nullptr;
	const Mesh* pActiveMesh = nullptr;
	const Shader* pActiveVertexShader = nullptr;
	const Shader* pActiveFragmentShader = nullptr;

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	DirectX::XMMATRIX matWorld;
	DirectX::XMMATRIX matView;
	DirectX::XMMATRIX matProj;

	std::unique_ptr<Shader> errorVertexShader;
	std::unique_ptr<Shader> errorPixelShader;

	bool initialized = false;
};
