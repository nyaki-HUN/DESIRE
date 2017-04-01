#pragma once

#include "Core/Singleton.h"

class IWindow;
class Mesh;
class Texture;

class Render
{
	DESIRE_DECLARE_SINGLETON(Render)

public:
	void Init(IWindow *mainWindow);
	void Kill();

	void UpdateRenderWindow(IWindow *window);

	void BeginFrame(IWindow *window);
	void EndFrame();

	void Bind(Mesh *mesh);
	void UnBind(Mesh *mesh);

	void Bind(Texture *texture);
	void Unbind(Texture *texture);

	static bool IsUsingOpenGL();

private:
	bool initialized;
};
