#pragma once

#include "Engine/Application/Application.h"

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
	enum class ERadioButtonOption
	{
		A,
		B,
		C
	};

	std::unique_ptr<FileSystemWatcher> dataDirWatcher;

	Object* scriptedObject = nullptr;
	Object* cubeObj = nullptr;

	InputMapping inputMapping;
};
