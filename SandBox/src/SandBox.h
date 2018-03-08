#pragma once

#include "Engine/Core/IApp.h"
#include "Engine/Input/InputMapping.h"

#include <memory>

class Object;
class FileSystemWatcher;

class SandBox : public IApp
{
public:
	SandBox();
	~SandBox() override;

	void Init() override;
	void Kill() override;
	void Update() override;

private:
	Object *scriptedObject = nullptr;
	Object *cubeObj = nullptr;

	InputMapping inputMapping;

	std::unique_ptr<FileSystemWatcher> dataDirWatcher;
};
