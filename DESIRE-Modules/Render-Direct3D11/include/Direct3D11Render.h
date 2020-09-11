#pragma once

#include "Engine/Render/Render.h"
#include "Engine/Utils/stl_utils.h"

#include <d3d11.h>
#include <DirectXMath.h>
#include <unordered_map>

class ShaderRenderDataD3D11;

class Direct3D11Render : public Render
{
public:
	Direct3D11Render();
	~Direct3D11Render() override;

	void Init(OSWindow& mainWindow) override;
	void UpdateRenderWindow(OSWindow& window) override;
	void Kill() override;

	void AppendShaderFilenameWithPath(WritableString& outString, const String& shaderFilename) const override;

	void EndFrame() override;

	void ClearActiveRenderTarget(uint32_t clearColorRGBA = 0x000000FF, float depth = 1.0f, uint8_t stencil = 0) override;

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

	void CreateBackBuffer(uint32_t width, uint32_t height);

	void SetViewport(uint16_t x, uint16_t y, uint16_t width, uint16_t height) override;

	void SetMesh(Mesh* pMesh) override;
	void UpdateDynamicMesh(DynamicMesh& dynamicMesh) override;
	void SetVertexShader(Shader* pVertexShader) override;
	void SetFragmentShader(Shader* pFragmentShader) override;
	void SetTexture(uint8_t samplerIdx, const Texture& texture, EFilterMode filterMode, EAddressMode addressMode) override;
	void SetRenderTarget(RenderTarget* pRenderTarget) override;
	void UpdateShaderParams(const Material& material) override;
	void UpdateShaderParams(const Material& material, const ShaderRenderDataD3D11* pShaderRenderData);
	static bool CheckAndUpdateShaderParam(const void* pValue, void* pValueInConstantBuffer, uint32_t size);

	void DoRender(uint32_t indexOffset, uint32_t vertexOffset, uint32_t numIndices, uint32_t numVertices) override;

	void UpdateD3D11Resource(ID3D11Resource* pResource, const void* pData, size_t size);
	void SetDepthStencilState();
	void SetRasterizerState();
	void SetBlendState();
	void SetInputLayout();
	void SetSamplerState(uint8_t samplerIdx, const D3D11_SAMPLER_DESC& samplerDesc);

	static DXGI_FORMAT GetTextureFormat(const Texture* pTexture);

	ID3D11Device* d3dDevice = nullptr;
	ID3D11DeviceContext* deviceCtx = nullptr;
	IDXGISwapChain* swapChain = nullptr;
	ID3D11RenderTargetView* pBackBufferRenderTargetView = nullptr;
	ID3D11DepthStencilView* pBackBufferDepthStencilView = nullptr;

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	D3D11_BLEND_DESC blendDesc = {};

	const ID3D11DepthStencilState* pActiveDepthStencilState = nullptr;
	const ID3D11RasterizerState* pActiveRasterizerState = nullptr;
	const ID3D11BlendState* pActiveBlendState = nullptr;
	const ID3D11InputLayout* pActiveInputLayout = nullptr;
	const ID3D11SamplerState* activeSamplerStates[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT] = {};

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	DirectX::XMMATRIX matWorld;
	DirectX::XMMATRIX matView;
	DirectX::XMMATRIX matProj;

	std::unordered_map<uint64_t, ID3D11DepthStencilState*> depthStencilStateCache;
	std::unordered_map<uint64_t, ID3D11RasterizerState*> rasterizerStateCache;
	std::unordered_map<uint64_t, ID3D11BlendState*> blendStateCache;
	std::unordered_map<std::pair<uint64_t, uint64_t>, ID3D11InputLayout*, stl_utils::hash_pair<uint64_t, uint64_t>> inputLayoutCache;
	std::unordered_map<uint64_t, ID3D11SamplerState*> samplerStateCache;

	std::unique_ptr<Shader> errorVertexShader;
	std::unique_ptr<Shader> errorPixelShader;

	bool initialized = false;
};
