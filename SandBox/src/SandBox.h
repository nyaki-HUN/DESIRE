#pragma once

#include "Core/IApp.h"
#include "Input/InputMap.h"

class Object;

class SandBox : public IApp
{
public:
	SandBox();
	~SandBox();

	void Init(IWindow *mainWindow) override;
	void Kill() override;
	void Update() override;

private:
	IWindow *window;
	Object *scriptedObject;
	Object *cubeObj;

	InputMap inputMap;
};
