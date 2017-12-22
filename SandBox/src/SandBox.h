#pragma once

#include "Core/IApp.h"
#include "Input/InputMapping.h"

#include <memory>

class Object;
class FileSystemWatcher;

class SandBox : public IApp
{
public:
	SandBox();
	~SandBox() override;

	void Init(IWindow *mainWindow) override;
	void Kill() override;
	void Update() override;

private:
	IWindow *window;
	Object *scriptedObject;
	Object *cubeObj;

	InputMapping inputMapping;

	std::unique_ptr<FileSystemWatcher> dataDirWatcher;
};
