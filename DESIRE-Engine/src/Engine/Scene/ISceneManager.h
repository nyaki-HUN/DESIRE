#pragma once

class Camera;
class RenderComponent;

class ISceneManager
{
protected:
	ISceneManager() {}

public:
	virtual ~ISceneManager() {}

	virtual void Add(RenderComponent* component, bool dynamic) = 0;
	virtual void Remove(RenderComponent* component) = 0;

	virtual void SetActiveCamera(Camera* camera) = 0;

	virtual void Update() = 0;
	virtual void Reset() = 0;
};
