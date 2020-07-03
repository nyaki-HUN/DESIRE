#pragma once

#include "Engine/Application/Application.h"

#include "Engine/Core/String/DynamicString.h"

#include "Engine/Input/InputMapping.h"

class FileSystemWatcher;
class Object;

class SandBox : public Application
{
public:
	SandBox();

	void Init() override;
	void Kill() override;
	void Update() override;

private:
	std::unique_ptr<FileSystemWatcher> dataDirWatcher;

	Object* scriptedObject = nullptr;
	Object* cubeObj = nullptr;

	InputMapping inputMapping;

	DynamicString textValue;
	float sliderValue = 0.0f;
	float color[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
};
