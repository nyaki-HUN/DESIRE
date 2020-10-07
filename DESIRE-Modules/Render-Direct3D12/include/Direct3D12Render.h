#pragma once

#include "Engine/Render/Render.h"

#include <d3d12.h>
#include <DirectXMath.h>

class ShaderRenderDataD3D12;
struct IDXGISwapChain3;

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
	void UpdateShaderParams(const Material& material, const ShaderRenderDataD3D12* pShaderRenderData);

	void DoRender(Renderable& renderable, uint32_t indexOffset, uint32_t vertexOffset, uint32_t numIndices, uint32_t numVertices) override;

	bool CreateFrameBuffers(uint32_t width, uint32_t height);
	void WaitForPreviousFrame();

	struct FrameBuffer
	{
		ID3D12Resource* m_pRenderTarget = nullptr;
		D3D12_CPU_DESCRIPTOR_HANDLE m_RTV = {};
		ID3D12CommandAllocator* m_pCommandAllocator = nullptr;
		ID3D12Fence* m_pFence = nullptr;
		uint64_t m_fenceValue = 0;
	};

	static constexpr uint32_t kFrameBufferCount = 3;

	ID3D12Device3* m_pDevice = nullptr;
	ID3D12CommandQueue* m_pCommandQueue = nullptr;
	IDXGISwapChain3* m_pSwapChain = nullptr;

	// Frame buffers
	ID3D12DescriptorHeap* m_pHeapForFrameBufferRTVs = nullptr;
	FrameBuffer m_frameBuffers[kFrameBufferCount];
	uint32_t m_activeFrameBufferIdx = 0;
	HANDLE m_fenceEvent;

	ID3D12DescriptorHeap* m_pHeapForDSV = nullptr;
	ID3D12Resource* m_pDepthStencilResource = nullptr;

	ID3D12GraphicsCommandList* m_pCmdList = nullptr;

	D3D12_DEPTH_STENCIL_DESC m_depthStencilDesc = {};
	D3D12_RASTERIZER_DESC m_rasterizerDesc = {};
	D3D12_BLEND_DESC m_blendDesc = {};

	float m_blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	DirectX::XMMATRIX m_matWorld;
	DirectX::XMMATRIX m_matView;
	DirectX::XMMATRIX m_matProj;

	std::unique_ptr<Shader> m_errorVertexShader;
	std::unique_ptr<Shader> m_errorPixelShader;
};
