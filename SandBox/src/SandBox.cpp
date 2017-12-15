#include "SandBox.h"
#include "SimpleRotateScript.h"
#include "Core/Log.h"
#include "Core/fs/FileSystem.h"
#include "Core/fs/FileSystemWatcher.h"
#include "Core/fs/IReadFile.h"
#include "Input/Input.h"
#include "Render/IRender.h"
#include "Resource/ResourceManager.h"
#include "Scene/Object.h"
#include "Script/IScriptSystem.h"
#include "Component/ScriptComponent.h"

#include "UI-imgui/src/ImGuiImpl.h"

enum EAction
{
	EXIT,
	FIRE,
	CAMERA_YAW,
	CAMERA_PITCH
}; 

SandBox::SandBox()
	: window(nullptr)
	, scriptedObject(nullptr)
	, cubeObj(nullptr)
{

}

SandBox::~SandBox()
{

}

void SandBox::Init(IWindow *mainWindow)
{
	window = mainWindow;

	ImGuiImpl::Get()->Init();

	FileSystem::Get()->AddZipFileSource("zip.zip");
//	FileSystem::Get()->AddZipFileSource("zip.zip", FileSystem::FLAG_IGNORE_CASE | FileSystem::FLAG_IGNORE_PATH);

	ReadFilePtr file = FileSystem::Get()->Open("asd.txt");
	ReadFilePtr file2 = FileSystem::Get()->Open("qwe.txt");
	ReadFilePtr file3 = FileSystem::Get()->Open("dir/qwe.txt");

	//////////

	scriptedObject = new Object("Obj_1");
	scriptedObject->AddComponent(IScriptSystem::Get()->CreateScriptComponent("TestScript"));

	ScriptComponent *scriptComp = scriptedObject->GetComponent<ScriptComponent>();
	if(scriptComp != nullptr)
	{
		scriptComp->CallByType(ScriptComponent::EBuiltinFuncType::INIT);

		scriptComp->Call("Function1");
		scriptComp->Call("Function2", 12.3f, 100);
		String stringValue = "TEST";
		scriptComp->Call("Function3", stringValue);
	}

	//////////

	REGISTER_NATIVE_SCRIPT(SimpleRotateScript);

	cubeObj = new Object("Obj_2");
	cubeObj->AddComponent(IScriptSystem::Get()->CreateScriptComponent("SimpleRotateScript"));

	//////////

	std::shared_ptr<Mesh> mesh = ResourceManager::Get()->GetMesh("data/meshes/sibenik/sibenik.obj");
	std::shared_ptr<Texture> texture = ResourceManager::Get()->GetTexture("data/meshes/sibenik/mramor6x6.png");

	//////////

	for(const Keyboard& keyboard : Input::Get()->GetKeyboards())
	{
		inputMapping.MapButton(EAction::EXIT, keyboard, KEY_ESCAPE);
		inputMapping.MapButton(EAction::FIRE, keyboard, KEY_SPACE);
	}

	for(const Mouse& mouse : Input::Get()->GetMouses())
	{
		inputMapping.MapButton(EAction::FIRE, mouse, Mouse::BUTTON_LEFT);
		inputMapping.MapAxis(EAction::CAMERA_YAW, mouse, Mouse::MOUSE_X);
		inputMapping.MapAxis(EAction::CAMERA_PITCH, mouse, Mouse::MOUSE_Y);
	}

	for(const GameController& gamepad : Input::Get()->GetControllers())
	{
		inputMapping.MapButton(EAction::FIRE, gamepad, GameController::BTN_X);
	}

	//////////

	dataDirWatcher = FileSystemWatcher::Create(FileSystem::Get()->GetAppDirectory() + "data", [](FileSystemWatcher::EAction action, const char *filename)
	{
		const char *strAction = "";
		switch(action)
		{
			case FileSystemWatcher::EAction::ADDED:		strAction = "added";  break;
			case FileSystemWatcher::EAction::DELETED:	strAction = "deleted"; break;
			case FileSystemWatcher::EAction::MODIFIED:	strAction = "modified"; break;
		}
		LOG_MESSAGE("%s %s", filename, strAction);
	});

	//////////


	LOG_DEBUG("Init done");
}

void SandBox::Kill()
{
	delete scriptedObject;

	ImGuiImpl::Get()->Kill();
}

void SandBox::Update()
{
	if(inputMapping.WentDown(EAction::EXIT))
	{
		LOG_MESSAGE("!EXIT!");
		return;
	}

	if(inputMapping.IsDown(EAction::FIRE))
	{
		LOG_MESSAGE("Fire is down");
	}

	dataDirWatcher->Update();

	IRender::Get()->BeginFrame(window);
	ImGuiImpl::Get()->NewFrame(window);

	ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiCond_FirstUseEver);
	ImGui::Begin("Another Window");
	ImGui::Text("Hello");
	ImGui::End();

	ImGuiImpl::Get()->EndFrame();
	IRender::Get()->EndFrame();
}
