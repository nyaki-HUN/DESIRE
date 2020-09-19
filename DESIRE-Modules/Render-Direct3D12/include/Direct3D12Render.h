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

	bool Init(OSWindow& mainWindow) override;
	void UpdateRenderWindow(OSWindow& window) override;
	void Kill() override;

	void AppendShaderFilenameWithPath(WritableString& outString, const String& shaderFilename) const override;

	void EndFrame() override;

	void Clear(uint32_t clearColorRGBA = 0x000000FF, float depth = 1.0f, uint8_t stencil = 0) override;

	void SetWorldMatrix(const Matrix4& matrix) override;
	void SetViewProjectionMatrices(const Matrix4& viewMatrix, const Matrix4& projMatrix) override;

	void SetScissor(uint16_t x = 0, uint16_t y = 0, uint16_t width = 0, uint16_t height = 0) override;
	void SetColorWriteEnabled(bool r, bool g, bool b, bool a) override;
	void SetDepthWriteEnabled(bool enabled) override;
	void SetDepthTest(EDepthTest depthTest) override;
	void SetCullMode(ECullMode cullMode) override;
	void SetBlendModeSeparated(EBlend srcBlendRGB, EBlend destBlendRGB, EBlendOp blendOpRGB, EBlend srcBlendAlpha, EBlend destBlendAlpha, EBlendOp blendOpAlpha) override;
	void SetBlendModeDisabled() override;

private:
	void* CreateMeshRenderData(const Mesh* pMesh) override;
	void* CreateShaderRenderData(const Shader* pShader) override;
	void* CreateTextureRenderData(const Texture* pTexture) override;
	void* CreateRenderTargetRenderData(const RenderTarget& renderTarget) override;

	void DestroyMeshRenderData(void* pRenderData) override;
	void DestroyShaderRenderData(void* pRenderData) override;
	void DestroyTextureRenderData(void* pRenderData) override;
	void DestroyRenderTargetRenderData(void* pRenderData) override;

	void SetMesh(Mesh* pMesh) override;
	void UpdateDynamicMesh(DynamicMesh& dynamicMesh) override;
	void SetVertexShader(Shader& vertexShader) override;
	void SetFragmentShader(Shader& fragmentShader) override;
	void SetTexture(uint8_t samplerIdx, const Texture& texture, EFilterMode filterMode, EAddressMode addressMode) override;
	void SetRenderTarget(RenderTarget* pRenderTarget) override;
	void UpdateShaderParams(const Material& material) override;
	void UpdateShaderParams(const Material& material, const ShaderRenderDataD3D12* pShaderRenderData);
	static bool CheckAndUpdateShaderParam(const void* pValue, void* pValueInConstantBuffer, uint32_t size);

	void DoRender(Renderable& renderable, uint32_t indexOffset, uint32_t vertexOffset, uint32_t numIndices, uint32_t numVertices) override;

	static DXGI_FORMAT GetTextureFormat(const Texture* pTexture);

	ID3D12Device3* d3dDevice = nullptr;
	ID3D12CommandQueue* commandQueue = nullptr;
	IDXGISwapChain1* swapChain = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE backBufferRenderTargetDescriptor = {};
	D3D12_CPU_DESCRIPTOR_HANDLE backBufferDepthStencilDescriptor = {};

	ID3D12GraphicsCommandList* pCmdList = nullptr;

	D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
	D3D12_RASTERIZER_DESC rasterizerDesc = {};
	D3D12_BLEND_DESC blendDesc = {};

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	DirectX::XMMATRIX matWorld;
	DirectX::XMMATRIX matView;
	DirectX::XMMATRIX matProj;

	std::unique_ptr<Shader> errorVertexShader;
	std::unique_ptr<Shader> errorPixelShader;
};
