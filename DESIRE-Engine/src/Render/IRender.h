#pragma once

#include "Core/Singleton.h"

class IWindow;
class Mesh;
class Material;
class Texture;

class IRender
{
	DESIRE_DECLARE_SINGLETON_INTERFACE(IRender)

public:
	virtual void Init(IWindow *mainWindow) = 0;
	virtual void Kill() = 0;

	virtual void UpdateRenderWindow(IWindow *window) = 0;

	virtual void BeginFrame(IWindow *window) = 0;
	virtual void EndFrame() = 0;

	// Mesh setup
	virtual void Bind(Mesh *mesh) = 0;
	virtual void Unbind(Mesh *mesh) = 0;
	virtual void SetMesh(Mesh *mesh) = 0;

	// Material setup
	virtual void Bind(Material *material) = 0;
	virtual void Unbind(Material *material) = 0;
	virtual void SetMaterial(Material *material) = 0;

	// Texture setup
	virtual void Bind(Texture *texture) = 0;
	virtual void Unbind(Texture *texture) = 0;
	virtual void SetTexture(uint8_t samplerIdx, Texture *texture) = 0;

	// Render state setup
	virtual void SetViewport(uint16_t x, uint16_t y, uint16_t width, uint16_t height) = 0;
	virtual void SetScissor(uint16_t x, uint16_t y, uint16_t width, uint16_t height) = 0;
};
