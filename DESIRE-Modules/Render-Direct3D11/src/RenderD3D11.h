#pragma once

#include "Render/IRender.h"
#include "Resource/Texture.h"
#include "Core/STL_utils.h"

#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>
#include <unordered_map>

class MeshRenderDataD3D11;
class ShaderRenderDataD3D11;

class RenderD3D11 : public IRender
{
public:
	RenderD3D11();
	~RenderD3D11() override;

	void Init(IWindow *mainWindow) override;
	void UpdateRenderWindow(IWindow *window) override;
	void Kill() override;

	String GetShaderFilenameWithPath(const char *shaderFilename) const override;

	void BeginFrame(IWindow *window) override;
	void EndFrame() override;

	void SetView(View *view) override;

	void SetWorldMatrix(const Matrix4& matrix) override;
	void SetViewProjectionMatrices(const Matrix4& viewMatrix, const Matrix4& projMatrix) override;

	void SetScissor(uint16_t x = 0, uint16_t y = 0, uint16_t width = 0, uint16_t height = 0) override;
	void SetClearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) override;
	void SetColorWriteEnabled(bool rgbWriteEnabled, bool alphaWriteEnabled) override;
	void SetDepthWriteEnabled(bool enabled) override;
	void SetDepthTest(EDepthTest deptTest) override;
	void SetCullMode(ECullMode cullMode) override;

	void Bind(Mesh *mesh) override;
	void Bind(Shader *shader) override;
	void Bind(Texture *texture) override;
	void Bind(RenderTarget *renderTarget) override;

	void Unbind(Mesh *mesh) override;
	void Unbind(Shader *shader) override;
	void Unbind(Texture *texture) override;
	void Unbind(RenderTarget *renderTarget) override;

	void UpdateDynamicMesh(DynamicMesh *mesh) override;

private:
	void SetViewport(uint16_t x, uint16_t y, uint16_t width, uint16_t height) override;

	void SetMesh(Mesh *mesh) override;
	void SetScreenSpaceQuadMeshAndVertexShader() override;
	void SetVertexShader(Shader *vertexShader) override;
	void SetFragmentShader(Shader *fragmentShader) override;
	void SetTexture(uint8_t samplerIdx, Texture *texture, EFilterMode filterMode, EAddressMode addressMode) override;
	void UpdateShaderParams() override;

	void DoRender() override;

	void UpdateD3D11Resource(ID3D11Resource *resource, const void *data, size_t size);
	void SetDepthStencilState();
	void SetRasterizerState();
	void SetBlendState();
	void SetInputLayout();
	void SetSamplerState(uint8_t samplerIdx, const D3D11_SAMPLER_DESC& samplerDesc);

	static DXGI_FORMAT ConvertTextureFormat(Texture::EFormat textureFormat);

	ID3D11Device *d3dDevice = nullptr;
	ID3D11DeviceContext *deviceCtx = nullptr;
	IDXGISwapChain *swapChain = nullptr;
	ID3D11RenderTargetView *backBufferRenderTargetView = nullptr;
	ID3D11DepthStencilView *backBufferDepthStencilView = nullptr;

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	D3D11_BLEND_DESC blendDesc = {};

	const ID3D11DepthStencilState *activeDepthStencilState = nullptr;
	const ID3D11RasterizerState *activeRasterizerState = nullptr;
	const ID3D11BlendState *activeBlendState = nullptr;
	const ID3D11InputLayout *activeInputLayout = nullptr;
	const ID3D11SamplerState *activeSamplerStates[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT] = {};
	const IWindow *activeWindow = nullptr;
	const Mesh *activeMesh = nullptr;
	const Shader *activeVertexShader = nullptr;
	const Shader *activeFragmentShader = nullptr;

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
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
	std::unique_ptr<Shader> screenSpaceQuadVertexShader;

	bool initialized = false;
};
