#pragma once

#include "Engine/Render/Render.h"
#include "Engine/Utils/stl_utils.h"

#include <DirectXMath.h>
#include <unordered_map>

class ShaderRenderDataD3D11;

struct D3D11_SAMPLER_DESC;
struct ID3D11BlendState;
struct ID3D11DepthStencilState;
struct ID3D11DepthStencilView;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11InputLayout;
struct ID3D11RasterizerState;
struct ID3D11RenderTargetView;
struct ID3D11Resource;
struct ID3D11SamplerState;
struct IDXGISwapChain;

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

	void Clear(uint32_t clearColorRGBA, float depth, uint8_t stencil) override;
	void SetScissor(uint16_t x, uint16_t y, uint16_t width, uint16_t height) override;
	void SetWorldMatrix(const Matrix4& matrix) override;
	void SetViewProjectionMatrices(const Matrix4& viewMatrix, const Matrix4& projMatrix) override;

private:
	RenderData* CreateRenderableRenderData(const Renderable& renderable) override;
	RenderData* CreateMeshRenderData(const Mesh& mesh) override;
	RenderData* CreateShaderRenderData(const Shader& shader) override;
	RenderData* CreateTextureRenderData(const Texture& texture) override;
	RenderData* CreateRenderTargetRenderData(const RenderTarget& renderTarget) override;

	void OnDestroyRenderableRenderData(RenderData* pRenderData) override;
	void OnDestroyShaderRenderData(RenderData* pRenderData) override;

	void SetMesh(Mesh& mesh) override;
	void UpdateDynamicMesh(DynamicMesh& dynamicMesh) override;
	void SetRenderTarget(RenderTarget* pRenderTarget) override;
	void UpdateShaderParams(const Material& material) override;
	void UpdateShaderParams(const Material& material, ShaderRenderDataD3D11* pShaderRenderData);

	void DoRender(Renderable& renderable, uint32_t indexOffset, uint32_t vertexOffset, uint32_t numIndices, uint32_t numVertices) override;

	bool CreateFrameBuffers(uint32_t width, uint32_t height);
	void UpdateD3D11Resource(ID3D11Resource* pResource, const void* pData, size_t size);
	void SetSamplerState(uint8_t samplerIdx, const D3D11_SAMPLER_DESC& samplerDesc);

	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pDeviceCtx = nullptr;
	IDXGISwapChain* m_pSwapChain = nullptr;
	ID3D11RenderTargetView* m_pFrameBufferRTV = nullptr;
	ID3D11DepthStencilView* m_pFrameBufferDSV = nullptr;

	const ShaderRenderDataD3D11* m_pActiveVS = nullptr;
	const ShaderRenderDataD3D11* m_pActivePS = nullptr;
	const ID3D11InputLayout* m_pActiveInputLayout = nullptr;
	const ID3D11RasterizerState* m_pActiveRasterizerState = nullptr;
	const ID3D11BlendState* m_pActiveBlendState = nullptr;
	const ID3D11DepthStencilState* m_pActiveDepthStencilState = nullptr;
	const ID3D11SamplerState* m_activeSamplerStates[kMaxTextureSamplers] = {};

	float m_blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	DirectX::XMMATRIX m_matWorld;
	DirectX::XMMATRIX m_matView;
	DirectX::XMMATRIX m_matProj;

	std::unordered_map<std::pair<uint64_t, uint64_t>, ID3D11InputLayout*, stl_utils::hash_pair<uint64_t, uint64_t>> m_inputLayoutCache;
	std::unordered_map<uint64_t, ID3D11DepthStencilState*> m_depthStencilStateCache;
	std::unordered_map<uint64_t, ID3D11RasterizerState*> m_rasterizerStateCache;
	std::unordered_map<uint64_t, ID3D11BlendState*> m_blendStateCache;
	std::unordered_map<uint64_t, ID3D11SamplerState*> m_samplerStateCache;

	std::unique_ptr<Shader> m_spErrorVertexShader;
	std::unique_ptr<Shader> m_spErrorPixelShader;
};
