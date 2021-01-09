#pragma once

class Camera;
class RenderComponent;

class ISceneManager
{
protected:
	ISceneManager() {}

public:
	virtual ~ISceneManager() {}

	virtual void Add(RenderComponent* pRenderComponent, bool dynamic) = 0;
	virtual void Remove(RenderComponent* pRenderComponent) = 0;

	virtual void SetActiveCamera(Camera* pCamera) = 0;

	virtual void Update() = 0;
	virtual void Reset() = 0;
};
