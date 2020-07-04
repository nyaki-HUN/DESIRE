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

	bool isCheckboxChecked = false;
	ERadioButtonOption radioButtonOption = ERadioButtonOption::A;
	float sliderValue = 0.0f;
	int32_t spinnerValue = 0;
	DynamicString textValue;
	float color[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
};
