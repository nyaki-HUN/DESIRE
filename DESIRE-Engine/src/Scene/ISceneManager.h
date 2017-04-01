#pragma once

#include "Core/Singleton.h"

class SceneNodeComponent;
class Camera;

class ISceneManager
{
	DESIRE_DECLARE_SINGLETON_INTERFACE(ISceneManager)

public:
	virtual void AddComponent(SceneNodeComponent *component, bool dynamic) = 0;
	virtual void RemoveComponent(SceneNodeComponent *component) = 0;

	virtual void SetActiveCamera(Camera *camera) = 0;

	virtual void Update() = 0;
	virtual void Reset() = 0;
};
