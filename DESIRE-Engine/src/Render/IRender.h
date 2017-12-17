#pragma once

#include "Core/Singleton.h"
#include "Core/String.h"

class IWindow;
class View;
class Mesh;
class DynamicMesh;
class Material;
class Shader;
class Texture;
class RenderTarget;
class Matrix4;

class IRender
{
	DESIRE_DECLARE_SINGLETON_INTERFACE(IRender)

public:
	enum class EDepthTest
	{
		DISABLED,
		LESS,
		LESS_EQUAL,
		GREATER,
		GREATER_EQUAL,
		EQUAL,
		NOT_EQUAL
	};

	enum class ECullMode
	{
		NONE,
		CCW,
		CW
	};

	virtual void Init(IWindow *mainWindow) = 0;
	virtual void UpdateRenderWindow(IWindow *window) = 0;
	virtual void Kill() = 0;

	virtual String GetShaderFilenameWithPath(const char *shaderFilename) const = 0;

	virtual void BeginFrame(IWindow *window) = 0;
	virtual void EndFrame() = 0;

	void RenderMesh(Mesh *mesh, Material *material);

	// Sets the current view that the rendering will happen on. (Use nullptr to set the default view which is using the frame buffer)
	virtual void SetView(View *view) = 0;

	virtual void SetWorldMatrix(const Matrix4& worldMatrix) = 0;
	virtual void SetViewProjectionMatrices(const Matrix4& viewMatrix, const Matrix4& projMatrix) = 0;

	// Render state setup
	virtual void SetScissor(uint16_t x = 0, uint16_t y = 0, uint16_t width = 0, uint16_t height = 0) = 0;
	virtual void SetClearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) = 0;
	virtual void SetColorWriteEnabled(bool rgbWriteEnabled, bool alphaWriteEnabled) = 0;
	virtual void SetDepthWriteEnabled(bool enabled) = 0;
	virtual void SetDepthTest(EDepthTest deptTest) = 0;
	virtual void SetCullMode(ECullMode cullMode) = 0;

	// Resource bind
	virtual void Bind(Mesh *mesh) = 0;
	virtual void Bind(Shader *shader) = 0;
	virtual void Bind(Texture *texture) = 0;
	virtual void Bind(RenderTarget *renderTarget) = 0;

	// Resource unbind
	virtual void Unbind(Mesh *mesh) = 0;
	virtual void Unbind(Shader *shader) = 0;
	virtual void Unbind(Texture *texture) = 0;
	virtual void Unbind(RenderTarget *renderTarget) = 0;

	// Resource update
	virtual void UpdateDynamicMesh(DynamicMesh *mesh) = 0;

protected:
	void SetDefaultRenderStates();

	virtual void SetViewport(uint16_t x, uint16_t y, uint16_t width, uint16_t height) = 0;

private:
	virtual void SetMesh(Mesh *mesh) = 0;
	virtual void SetShadersFromMaterial(Material *material) = 0;
	virtual void SetTexture(uint8_t samplerIdx, Texture *texture) = 0;

	// Submit draw command
	virtual void DoRender() = 0;
};
