#pragma once

#include "Engine/Application/Application.h"
#include "Engine/Input/InputMapping.h"

#include <memory>

class ImGuiImpl;
class FileSystemWatcher;
class Object;

class SandBox : public Application
{
public:
	SandBox();
	~SandBox() override;

	void Init() override;
	void Kill() override;
	void Update() override;

private:
	std::unique_ptr<ImGuiImpl> UI;
	std::unique_ptr<FileSystemWatcher> dataDirWatcher;

	Object *scriptedObject = nullptr;
	Object *cubeObj = nullptr;

	InputMapping inputMapping;
};
