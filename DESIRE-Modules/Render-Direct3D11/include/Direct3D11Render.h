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
	void* CreateRenderableRenderData(const Renderable& renderable) override;
	void* CreateMeshRenderData(const Mesh& mesh) override;
	void* CreateShaderRenderData(const Shader& shader) override;
	void* CreateTextureRenderData(const Texture& texture) override;
	void* CreateRenderTargetRenderData(const RenderTarget& renderTarget) override;

	void DestroyRenderableRenderData(void* pRenderData) override;
	void DestroyMeshRenderData(void* pRenderData) override;
	void DestroyShaderRenderData(void* pRenderData) override;
	void DestroyTextureRenderData(void* pRenderData) override;
	void DestroyRenderTargetRenderData(void* pRenderData) override;

	void SetMesh(Mesh& mesh) override;
	void UpdateDynamicMesh(DynamicMesh& dynamicMesh) override;
	void SetTexture(uint8_t samplerIdx, const Texture& texture, EFilterMode filterMode, EAddressMode addressMode) override;
	void SetRenderTarget(RenderTarget* pRenderTarget) override;
	void UpdateShaderParams(const Material& material) override;
	void UpdateShaderParams(const Material& material, const ShaderRenderDataD3D11* pShaderRenderData);

	void DoRender(Renderable& renderable, uint32_t indexOffset, uint32_t vertexOffset, uint32_t numIndices, uint32_t numVertices) override;

	bool CreateFrameBuffers(uint32_t width, uint32_t height);
	void UpdateD3D11Resource(ID3D11Resource* pResource, const void* pData, size_t size);
	void SetDepthStencilState();
	void SetRasterizerState();
	void SetBlendState();
	void SetInputLayout();
	void SetSamplerState(uint8_t samplerIdx, const D3D11_SAMPLER_DESC& samplerDesc);

	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pDeviceCtx = nullptr;
	IDXGISwapChain* m_pSwapChain = nullptr;
	ID3D11RenderTargetView* m_pFrameBufferRTV = nullptr;
	ID3D11DepthStencilView* m_pFrameBufferDSV = nullptr;

	D3D11_DEPTH_STENCIL_DESC m_depthStencilDesc = {};
	D3D11_RASTERIZER_DESC m_rasterizerDesc = {};
	D3D11_BLEND_DESC m_blendDesc = {};

	const ShaderRenderDataD3D11* m_pActiveVS = nullptr;
	const ShaderRenderDataD3D11* m_pActivePS = nullptr;
	const ID3D11DepthStencilState* m_pActiveDepthStencilState = nullptr;
	const ID3D11RasterizerState* m_pActiveRasterizerState = nullptr;
	const ID3D11BlendState* m_pActiveBlendState = nullptr;
	const ID3D11InputLayout* m_pActiveInputLayout = nullptr;
	const ID3D11SamplerState* m_activeSamplerStates[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT] = {};

	float m_blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	DirectX::XMMATRIX m_matWorld;
	DirectX::XMMATRIX m_matView;
	DirectX::XMMATRIX m_matProj;

	std::unordered_map<uint64_t, ID3D11DepthStencilState*> m_depthStencilStateCache;
	std::unordered_map<uint64_t, ID3D11RasterizerState*> m_rasterizerStateCache;
	std::unordered_map<uint64_t, ID3D11BlendState*> m_blendStateCache;
	std::unordered_map<std::pair<uint64_t, uint64_t>, ID3D11InputLayout*, stl_utils::hash_pair<uint64_t, uint64_t>> m_inputLayoutCache;
	std::unordered_map<uint64_t, ID3D11SamplerState*> m_samplerStateCache;

	std::unique_ptr<Shader> m_errorVertexShader;
	std::unique_ptr<Shader> m_errorPixelShader;
};
