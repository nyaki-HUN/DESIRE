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
	std::unique_ptr<FileSystemWatcher> m_spDataDirWatcher;

	Object* m_pScriptedObject = nullptr;
	Object* m_pCubeObj = nullptr;

	InputMapping m_inputMapping;
};
