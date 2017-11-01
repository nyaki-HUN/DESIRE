#pragma once

#include "Render/IRender.h"

#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>
#include <unordered_map>

class MeshRenderDataD3D11;

class RenderD3D11 : public IRender
{
public:
	RenderD3D11();
	~RenderD3D11();

	void Init(IWindow *mainWindow) override;
	void UpdateRenderWindow(IWindow *window) override;
	void Kill() override;

	String GetShaderFilenameWithPath(const char *shaderFilename) const override;

	void BeginFrame(IWindow *window) override;
	void EndFrame() override;

	void SetView(View *view) override;

	void SetWorldMatrix(const Matrix4& matrix) override;
	void SetViewProjectionMatrices(const Matrix4& viewMatrix, const Matrix4& projMatrix) override;
	void SetScissor(uint16_t x, uint16_t y, uint16_t width, uint16_t height) override;

	void Bind(Mesh *mesh) override;
	void Bind(Shader *shader) override;
	void Bind(Texture *texture) override;

	void Unbind(Mesh *mesh) override;
	void Unbind(Shader *shader) override;
	void Unbind(Texture *texture) override;

	void UpdateDynamicMesh(DynamicMesh *mesh) override;

private:
	void SetMesh(Mesh *mesh) override;
	void SetShadersFromMaterial(Material *material) override;
	void SetTexture(uint8_t samplerIdx, Texture *texture) override;

	void SetViewport(uint16_t x, uint16_t y, uint16_t width, uint16_t height) override;

	void DoRender() override;

	void UpdateD3D11Resource(ID3D11Resource *resource, const void *data, size_t size);

	ID3D11Device *d3dDevice;
	IDXGISwapChain *swapChain;
	ID3D11RenderTargetView *renderTargetView;
	ID3D11DeviceContext *deviceCtx;

	std::unordered_map<uint64_t, ID3D11InputLayout*> inputLayoutCache;

	const Mesh *activeMesh;
	std::unique_ptr<Shader> errorVertexShader;
	std::unique_ptr<Shader> errorPixelShader;

	DirectX::XMMATRIX worldMatrix;
	float clearColor[4];

	bool initialized;
};
