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

	void SetMesh(Mesh& mesh) override;
	void UpdateDynamicMesh(DynamicMesh& dynamicMesh) override;
	void SetRenderTarget(RenderTarget* pRenderTarget) override;
	void UpdateShaderParams(const Material& material) override;
	void UpdateShaderParams(const Material& material, ShaderRenderDataD3D12* pShaderRenderData);

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

	ID3D12RootSignature* m_pRootSignature = nullptr;

	ID3D12PipelineState* m_pActivePipelineState = nullptr;

	float m_blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	DirectX::XMMATRIX m_matWorld;
	DirectX::XMMATRIX m_matView;
	DirectX::XMMATRIX m_matProj;

	std::unique_ptr<Shader> m_errorVertexShader;
	std::unique_ptr<Shader> m_errorPixelShader;
};
