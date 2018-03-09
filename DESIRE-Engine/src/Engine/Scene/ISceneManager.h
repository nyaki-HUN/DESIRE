#pragma once

class Object;
class Camera;

class ISceneManager
{
protected:
	ISceneManager() {}

public:
	virtual ~ISceneManager() {}

	virtual void AddObject(Object *obj, bool dynamic) = 0;
	virtual void RemoveObject(Object *obj) = 0;

	virtual void SetActiveCamera(Camera *camera) = 0;

	virtual void Update() = 0;
	virtual void Reset() = 0;
};
