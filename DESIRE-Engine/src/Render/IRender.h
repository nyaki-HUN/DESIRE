#pragma once

#include "Core/Singleton.h"
#include "Core/String.h"

class IWindow;
class Mesh;
class Material;
class Shader;
class Texture;
class Matrix4;

class IRender
{
	DESIRE_DECLARE_SINGLETON_INTERFACE(IRender)

public:
	virtual void Init(IWindow *mainWindow) = 0;
	virtual void UpdateRenderWindow(IWindow *window) = 0;
	virtual void Kill() = 0;

	virtual String GetShaderFilenameWithPath(const char *shaderFilename) const = 0;

	virtual void BeginFrame(IWindow *window) = 0;
	virtual void EndFrame() = 0;

	void RenderMesh(Mesh *mesh, Material *material);

	// Render state setup
	virtual void SetViewProjectionMatrices(const Matrix4& viewMatrix, const Matrix4& projMatrix) = 0;
	virtual void SetViewport(uint16_t x, uint16_t y, uint16_t width, uint16_t height) = 0;
	virtual void SetScissor(uint16_t x, uint16_t y, uint16_t width, uint16_t height) = 0;

	// Resource bind
	virtual void Bind(Mesh *mesh) = 0;
	virtual void Bind(Shader *shader) = 0;
	virtual void Bind(Texture *texture) = 0;

	// Resource unbind
	virtual void Unbind(Mesh *mesh) = 0;
	virtual void Unbind(Shader *shader) = 0;
	virtual void Unbind(Texture *texture) = 0;

protected:
	virtual void SetMesh(Mesh *mesh) = 0;
	virtual void SetShader(Shader *vertexShader, Shader *pixelShader) = 0;
	virtual void SetTexture(uint8_t samplerIdx, Texture *texture) = 0;

	// Submit draw command
	virtual void DoRender() = 0;

	const Mesh *activeMesh = nullptr;
	const Material *activeMaterial = nullptr;

	Shader *errorVertexShader = nullptr;
	Shader *errorPixelShader = nullptr;
};
