#pragma once

#include "Render/IRender.h"

#include <d3d11.h>
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

	void SetViewProjectionMatrices(const Matrix4& viewMatrix, const Matrix4& projMatrix) override;
	void SetViewport(uint16_t x, uint16_t y, uint16_t width, uint16_t height) override;
	void SetScissor(uint16_t x, uint16_t y, uint16_t width, uint16_t height) override;

	void Bind(Mesh *mesh) override;
	void Bind(Shader *shader) override;
	void Bind(Texture *texture) override;

	void Unbind(Mesh *mesh) override;
	void Unbind(Shader *shader) override;
	void Unbind(Texture *texture) override;

private:
	void SetMesh(Mesh *mesh) override;
	void SetShader(Shader *vertexShader, Shader *pixelShader) override;
	void SetTexture(uint8_t samplerIdx, Texture *texture) override;

	void DoRender() override;

	ID3D11Device *d3dDevice;
	IDXGISwapChain *swapChain;
	ID3D11RenderTargetView *renderTargetView;
	ID3D11DeviceContext *deviceCtx;

	std::unordered_map<uint64_t, ID3D11InputLayout*> inputLayoutCache;

	float clearColor[4];

	bool initialized;
};
