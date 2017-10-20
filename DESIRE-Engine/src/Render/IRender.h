#pragma once

#include "Core/Singleton.h"

class IWindow;
class Mesh;
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

	virtual void Bind(Mesh *mesh) = 0;
	virtual void Unbind(Mesh *mesh) = 0;

	virtual void Bind(Texture *texture) = 0;
	virtual void Unbind(Texture *texture) = 0;
	virtual void SetTexture(uint8_t samplerIdx, Texture *texture) = 0;
};
